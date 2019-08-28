#include "metadatadescriptor.h"
extern const ASDCP::Dictionary *g_Dict;

MetadataDescriptor::MetadataDescriptor(const DescriptorParams& parames)
    :params_(parames)
{

}

void MetadataDescriptor::createDescriptorFromFile(const QString &filename)
{

}

void MetadataDescriptor::createDescriptor(TSR::FrameBuffer* buffer)
{
    if(params_.outputColorSpace.contains("RGB", Qt::CaseInsensitive))s
        generic_essence_desciptor_ = new ASDCP::MXF::RGBAEssenceDescriptor(g_Dict);
     else if(params_.outputColorSpace.contains("YUV", Qt::CaseInsensitive))
         generic_essence_desciptor_ = new ASDCP::MXF::CDCIEssenceDescriptor(g_Dict);
    Kumu::GenRandomValue(generic_essence_desciptor_->InstanceUID);

    ASDCP::JP2K::CodestreamParser parser;
    ASDCP::JP2K::FrameBuffer      frameBuffer;
    frameBuffer.SetData(buffer->data(), buffer->size());
    frameBuffer.Size(buffer->size());
    ASDCP::JP2K::PictureDescriptor* picDesc = new ASDCP::JP2K::PictureDescriptor();
    Kumu::Result_t result = ASDCP::JP2K::ParseMetadataIntoDesc(frameBuffer, *picDesc);
    result = parser.FillPictureDescriptor(*picDesc);
    if(ASDCP_SUCCESS(result))
    {
        createJPEG2000SubDescriptor(picDesc);
    }

     ASDCP::MXF::RGBAEssenceDescriptor* rgba_descriptor = dynamic_cast<ASDCP::MXF::RGBAEssenceDescriptor*>(generic_essence_desciptor_);
     ASDCP::MXF::CDCIEssenceDescriptor* cdci_descriptor = dynamic_cast<ASDCP::MXF::CDCIEssenceDescriptor*>(generic_essence_desciptor_);
     if(rgba_descriptor)
     {
         creatRGBADescriptor(*rgba_descriptor, params_.outputVideoDepth);
         SetGenericPictureEssenceDescriporDefaults(static_cast<ASDCP_V2::MXF::GenericPictureEssenceDescriptor*> (pRGBADesc), asset);
         if (m_app == APP_RnS)
         {
             pRGBADesc->CodingEquations.set_has_value(false);
         }
         else
         {
             pRGBADesc->CodingEquations.set_has_value(true);
         }

         if (appCtx.getCurrentCPL().m_info_advance.type == IMF_App5)
         {
             pRGBADesc->ComponentMaxRef.set_has_value(false);
             pRGBADesc->ComponentMinRef.set_has_value(false);
         }
         setJ2CLayout(pJ2kDesc, asset->m_dst_depth);
         setPictureComponentSizingForRGB(pJ2kDesc, asset->m_dst_depth);

         if (cpl_type == IMF_AppProRes)
         {
             // Plugfest 201810, anton@apple.com: In our test vector, we asked for the aspect ratio to be reduced (ie. like 16/9)
             int nG = IMFCreator_Math::GCD(pRGBADesc->StoredWidth, pRGBADesc->StoredHeight);
             pRGBADesc->AspectRatio = ASDCP_V2::Rational(pRGBADesc->StoredWidth/nG, pRGBADesc->StoredHeight/nG);
         }
     }
     else if(cdci_descriptor)
     {
         IColorimetry colorimetry = appCtx.getProject()->m_pProjectConfiguration.m_index_colorimetry;
         j2k_colorspace_t nColor  = GetColorSpaceFromProjectConfig(appCtx.getProject()->m_pProjectConfiguration); // just for test.
         ASDCP_V2::SetCDCIDescriptor(*pCDCIDesc, colorimetry, nColor, asset->m_dst_depth); // just set for the CDCI
         SetGenericPictureEssenceDescriporDefaults(static_cast<ASDCP_V2::MXF::GenericPictureEssenceDescriptor*>(pCDCIDesc), asset);
         if(cpl_type != IMF_App5)
         {
             setJ2CLayoutForYUV(pJ2kDesc, asset->m_dst_depth);
             setPictureComponentSizingForYUV(pJ2kDesc, nColor, asset->m_dst_depth);
         }

         if (cpl_type == IMF_AppProRes)
         {
             // Plugfest 201810, anton@apple.com: In our test vector, we asked for the aspect ratio to be reduced (ie. like 16/9)
             int nG = IMFCreator_Math::GCD(pCDCIDesc->StoredWidth, pCDCIDesc->StoredHeight);
             pCDCIDesc->AspectRatio = ASDCP_V2::Rational(pCDCIDesc->StoredWidth/nG, pCDCIDesc->StoredHeight/nG);
         }
     }
     setGenericEssenceDescriptorUL();
}

bool MetadataDescriptor::createJPEG2000SubDescriptor(ASDCP::JP2K::PictureDescriptor* pDesc)
{
    jpeg2000_sub_descriptor_ = new ASDCP::MXF::JPEG2000PictureSubDescriptor(g_Dict);
    Kumu::GenRandomValue(jpeg2000_sub_descriptor_->InstanceUID);
    if(pDesc != NULL)
    {
        jpeg2000_sub_descriptor_->Rsize   = params_.rSize;
        jpeg2000_sub_descriptor_->Xsize   = pDesc->Xsize;
        jpeg2000_sub_descriptor_->Ysize   = pDesc->Ysize;
        jpeg2000_sub_descriptor_->XOsize  = pDesc->XOsize;
        jpeg2000_sub_descriptor_->YOsize  = pDesc->YOsize;
        jpeg2000_sub_descriptor_->XTsize  = pDesc->XTsize;
        jpeg2000_sub_descriptor_->YTsize  = pDesc->YTsize;
        jpeg2000_sub_descriptor_->XTOsize = pDesc->XTOsize;
        jpeg2000_sub_descriptor_->YTOsize = pDesc->YTOsize;
        jpeg2000_sub_descriptor_->Csize   = pDesc->Csize;

        if(appCtx.getCurrentCPL().m_info_advance.hasJP2KParams)
        {
            if (this->m_app == APP_RnS)
            {
                JPSubDescriptor->QuantizationDefault.set_has_value(false);

                // FIXME: CodingStyleDefault should read from j2c buffer
                // TODO: read Base IMP video mxf Desc and set here
                pDesc->CodingStyleDefault.SGcod.MultiCompTransform = 1;
                pDesc->CodingStyleDefault.SGcod.NumberOfLayers[0] = 0;
                pDesc->CodingStyleDefault.SGcod.NumberOfLayers[1] = 1;
                pDesc->CodingStyleDefault.SGcod.ProgressionOrder = 4;
                pDesc->CodingStyleDefault.SPcod.CodeblockHeight = 3;
                pDesc->CodingStyleDefault.SPcod.CodeblockStyle = 0;
                pDesc->CodingStyleDefault.SPcod.CodeblockWidth = 3;
                pDesc->CodingStyleDefault.SPcod.DecompositionLevels = 5;
                pDesc->CodingStyleDefault.SPcod.CodeblockHeight = 3;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[0] = 119;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[1] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[2] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[3] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[4] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[5] = 136;
                pDesc->CodingStyleDefault.SPcod.Transformation = 1;
                pDesc->CodingStyleDefault.Scod = 1;
            }
            else
            {
                pJ2kDesc->QuantizationDefault.set(appCtx.getCurrentCPL().m_info_advance.quantizationDefault);
                pJ2kDesc->QuantizationDefault.set_has_value(true);
            }

            pJ2kDesc->CodingStyleDefault.set(appCtx.getCurrentCPL().m_info_advance.codingStyleDefault);
            pJ2kDesc->CodingStyleDefault.set_has_value(true);
        }
        else
        {
            const ui32_t tmp_buffer_len = 1024;
            byte_t tmp_buffer[tmp_buffer_len];

            *(ui32_t*)tmp_buffer = KM_i32_BE(MaxComponents); // three components
            *(ui32_t*)(tmp_buffer+4) = KM_i32_BE(sizeof(ASDCP_V2::JP2K::ImageComponent_t));
            memcpy(tmp_buffer + 8, &pDesc->ImageComponents, sizeof(ASDCP_V2::JP2K::ImageComponent_t) * MaxComponents);

            const ui32_t pcomp_size = (sizeof(ui32_t) * 2) + (sizeof(ASDCP_V2::JP2K::ImageComponent_t) * MaxComponents);
            memcpy(pJ2kDesc->PictureComponentSizing.get().Data(), tmp_buffer, pcomp_size);
            pJ2kDesc->PictureComponentSizing.get().Length(pcomp_size);
            pJ2kDesc->PictureComponentSizing.set_has_value();

            if (m_app == APP_RnS)
            {
                // FIXME: CodingStyleDefault should read from j2c buffer
                pDesc->CodingStyleDefault.SGcod.MultiCompTransform = 1;
                pDesc->CodingStyleDefault.SGcod.NumberOfLayers[0] = 0;
                pDesc->CodingStyleDefault.SGcod.NumberOfLayers[1] = 1;
                pDesc->CodingStyleDefault.SGcod.ProgressionOrder = 4;
                pDesc->CodingStyleDefault.SPcod.CodeblockHeight = 3;
                pDesc->CodingStyleDefault.SPcod.CodeblockStyle = 0;
                pDesc->CodingStyleDefault.SPcod.CodeblockWidth = 3;
                pDesc->CodingStyleDefault.SPcod.DecompositionLevels = 5;
                pDesc->CodingStyleDefault.SPcod.CodeblockHeight = 3;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[0] = 119;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[1] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[2] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[3] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[4] = 136;
                pDesc->CodingStyleDefault.SPcod.PrecinctSize[5] = 136;
                pDesc->CodingStyleDefault.SPcod.Transformation = 1;
                pDesc->CodingStyleDefault.Scod = 1;
            }

            ui32_t precinct_set_size = 0;
            for ( ui32_t i = 0; pDesc->CodingStyleDefault.SPcod.PrecinctSize[i] != 0 && i < MaxPrecincts; ++i )
                precinct_set_size++;

            ui32_t csd_size = sizeof(CodingStyleDefault_t) - MaxPrecincts + precinct_set_size;
            memcpy(pJ2kDesc->CodingStyleDefault.get().Data(), &pDesc->CodingStyleDefault, csd_size);
            pJ2kDesc->CodingStyleDefault.get().Length(csd_size);
            pJ2kDesc->CodingStyleDefault.set_has_value();

            if (this->m_app == APP_RnS)
            {
                pJ2kDesc->QuantizationDefault.set_has_value(false);
            }
            else
            {
                ui32_t qdflt_size = pDesc->QuantizationDefault.SPqcdLength + 1;
                memcpy(pJ2kDesc->QuantizationDefault.get().Data(), &pDesc->QuantizationDefault, qdflt_size);
                pJ2kDesc->QuantizationDefault.get().Length(qdflt_size);
                pJ2kDesc->QuantizationDefault.set_has_value();
            }
        }
        return;
    }
    else
    {
        pJ2kDesc->Rsize   = asset->rsize_target;
        pJ2kDesc->Xsize   = asset->storedwidth;
        pJ2kDesc->Ysize   = asset->storedheight;
        pJ2kDesc->XOsize  = 0;
        pJ2kDesc->YOsize  = 0;
        pJ2kDesc->XTsize  = asset->storedwidth;
        pJ2kDesc->YTsize  = asset->storedheight;
        pJ2kDesc->XTOsize = 0;
        pJ2kDesc->YTOsize = 0;
        pJ2kDesc->Csize   = 3;
    }
}
const int VideoLineMapSize = 16; // See SMPTE 377M D.2.1
const int PixelLayoutSize = 8*2; // See SMPTE 377M D.2.3
void MetadataDescriptor::createRGBADescriptor(ASDCP::MXF::RGBAEssenceDescriptor &rgba_desciptor, int pixDepth)
{
    // we just used QE.2 system
    // Component Max Ref
    switch(pixDepth)
    {
    case 8:
        rgba_desciptor.ComponentMaxRef = 255;
        rgba_desciptor.ComponentMinRef = 0;
        break;
    case 10:
        rgba_desciptor.ComponentMaxRef = 1023;
        rgba_desciptor.ComponentMinRef = 0;
        break;
    case 12:
        rgba_desciptor.ComponentMaxRef = 4095;
        rgba_desciptor.ComponentMinRef = 0;
        break;
    case 16:
        rgba_desciptor.ComponentMaxRef = 65535;
        rgba_desciptor.ComponentMinRef = 0;
        break;
    }
    rgba_desciptor.ScanningDirection.set(0);
    char pArray[ASDCP::MXF::RGBAValueLength];
    if(appCtx.getCurrentCPL().m_info_advance.type == IMF_App5)
    {
        pArray[0] = 'B', pArray[2] = 'G', pArray[4] = 'R';;
        pArray[1] = pArray[3] = pArray[5] = 253;
        memset(pArray + 6, 0, 10);
    }
    else
    {
        pArray[0] = 'R', pArray[2] = 'G', pArray[4] = 'B';
        pArray[1] = pArray[3] = pArray[5] = pixDepth;
        memset(pArray + 6, 0, 10);
    }
    rgba_desciptor.PixelLayout.Set(pArray);
}

void MetadataDescriptor::createGenericPictureEssenceDescripor()
{
    generic_essence_desciptor_->LinkedTrackID = 2; // Note : this is from DVS and Frauhofer IMP
    generic_essence_desciptor_->StoredWidth   = asset->storedwidth;
    generic_essence_desciptor_->StoredHeight  = asset->storedheight;
    generic_essence_desciptor_->SampledWidth  = pDesc->StoredWidth ;
    generic_essence_desciptor_->SampledHeight = pDesc->StoredHeight;
    generic_essence_desciptor_->SampledXOffset = 0;
    generic_essence_desciptor_->SampledYOffset = 0;
    generic_essence_desciptor_->ActiveWidth   = pDesc->StoredWidth;
    generic_essence_desciptor_->ActiveHeight  = pDesc->StoredHeight;
    generic_essence_desciptor_->ActiveXOffset = 0;
    generic_essence_desciptor_->ActiveYOffset = 0;
    generic_essence_desciptor_->DisplayWidth  = pDesc->StoredWidth;
    generic_essence_desciptor_->DisplayHeight = pDesc->StoredHeight;
    generic_essence_desciptor_->DisplayXOffset  = 0;
    generic_essence_desciptor_->DisplayYOffset  = 0;
    generic_essence_desciptor_->DisplayF2Offset = 0; // Shall be preset in App2
    if(appCtx.getCurrentCPL().m_info_advance.type == IMF_App5)
    {
        generic_essence_desciptor_->DisplayF2Offset.set_has_value(false);
    }


    /*NOTE: this is just for Fotokem's IMF that the BASE IMP is from colorfront.*/
    int nG = 1;// IMFCreator_Math::GCD(pDesc->StoredWidth, pDesc->StoredHeight);
    // and R&S (Clipster 6.5.42.4.1, 2018.1) is 16/9
    if (asset->m_mxf_base_app == APP_RnS)
    {
        generic_essence_desciptor_->ActiveWidth.set_has_value(false);
        generic_essence_desciptor_->ActiveHeight.set_has_value(false);
        generic_essence_desciptor_->ActiveXOffset.set_has_value(false);
        generic_essence_desciptor_->ActiveYOffset.set_has_value(false);
        generic_essence_desciptor_->DisplayF2Offset.set_has_value(false);
        generic_essence_desciptor_->CodingEquations.set_has_value(false);
        nG = IMFCreator_Math::GCD(pDesc->StoredWidth, pDesc->StoredHeight);
    }
    else
    {
        generic_essence_desciptor_->CodingEquations.set_has_value(true);
    }
    generic_essence_desciptor_->AspectRatio = ASDCP_V2::Rational(pDesc->StoredWidth/nG, pDesc->StoredHeight/nG);

    generic_essence_desciptor_->FrameLayout = 0;
    generic_essence_desciptor_->ScanningDirection = 0;
    generic_essence_desciptor_->ContainerDuration = asset->duration;
    LineMapPair line(42, 0); // color front use 42, we used 1
    generic_essence_desciptor_->VideoLineMap = line;
    generic_essence_desciptor_->SampleRate = string2rational(asset->edit_rate);
    Set2086Metadata(pDesc, asset->m_color_volume_md);
}
