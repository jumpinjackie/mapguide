using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    class Utils
    {
        internal static void LoadResource(MgResourceService resSvc, string resIdStr, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResource(resId, br, null);
        }

        internal static void LoadResourceData(MgResourceService resSvc, string resIdStr, string dataName, string dataType, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResourceData(resId, dataName, dataType, br);
        }

        internal static void CheckValidImage(MgByteReader img, int? imageWidth = null, int? imageHeight = null, string format = null)
        {
            try
            {
                MgByteSink sink = new MgByteSink(img);
                sink.ToFile("tile.img");
                using (Image image = Image.FromFile("tile.img"))
                {
                    Assert.IsNotNull(image);
                    if (imageWidth != null)
                        Assert.Equals(image.Width, imageWidth.Value);
                    if (imageHeight != null)
                        Assert.Equals(image.Height, imageHeight.Value);

                    if (format != null)
                    {
                        switch (format)
                        {
                            case MgImageFormats.Gif:
                                Assert.Equals(image.RawFormat, ImageFormat.Gif);
                                break;
                            case MgImageFormats.Jpeg:
                                Assert.Equals(image.RawFormat, ImageFormat.Jpeg);
                                break;
                            case MgImageFormats.Png:
                            case MgImageFormats.Png8:
                                Assert.Equals(image.RawFormat, ImageFormat.Png);
                                break;
                            case MgImageFormats.Tiff:
                                Assert.Equals(image.RawFormat, ImageFormat.Tiff);
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                throw new AssertException("Invalid image found: " + ex.Message);
            }
            finally
            {
                if (File.Exists("tile.img"))
                {
                    try
                    {
                        File.Delete("tile.img");
                    }
                    catch { }
                }
            }
        }
    }
}
