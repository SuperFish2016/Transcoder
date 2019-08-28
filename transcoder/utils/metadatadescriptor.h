#ifndef METADATADESCRIPTOR_H
#define METADATADESCRIPTOR_H
#include <AS_DCP.h>
#include <Metadata.h>
#include "buffer.h"
/**
* @brief
* MetadataDescriptor is used to create proper metadata descriptor for mxf file.
* There are two way to create the descriptor.
* a) read a existing mxf
* b) create a new descriptor according to the project settings.
*
* @author chentao
*/

QList<MetadataDescriptor*> CPLVideoMetadataDescriptor;
struct DescriptorParams
{
    QString outputColorSpace;
    qint32  outputVideoDepth;
};
class MetadataDescriptor
{
public:
    MetadataDescriptor(const DescriptorParams& parames);
    static MetadataDescriptor* create(const Asset* asset);

    bool createJPEG2000SubDescriptor(ASDCP::JP2K::PictureDescriptor* pictureDescriptor);
    bool createDescriptor(TSR::FrameBuffer* buffer);
    void createRGBADescriptor(ASDCP::MXF::RGBAEssenceDescriptor &pDesc, int pixDepth);
    void createGenericPictureEssenceDescripor();
    /* create Desciptor via parsing a existing mxf file. used in supplemental mode.*/
    bool createDescriptorFromFile(const QString& filename);

    ASDCP::MXF::GenericPictureEssenceDescriptor* getGenericPictureEssenceDescriptor();
    ASDCP::MXF::JPEG2000PictureSubDescriptor*    getJPEG2000PictureSubDescriptor();
    ASDCP::MXF::ACESPictureSubDescriptor*        getACESPictureSubDescriptor();
    private:
private:
    ASDCP::MXF::GenericPictureEssenceDescriptor* generic_essence_desciptor_;
    ASDCP::MXF::JPEG2000PictureSubDescriptor*  jpeg2000_sub_descriptor_;
    DescriptorParams params_;
};

#endif // METADATADESCRIPTOR_H
