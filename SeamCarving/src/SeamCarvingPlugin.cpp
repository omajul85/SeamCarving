#include "SeamCarvingPlugin.hpp"
#include "SeamCarvingProcess.hpp"
#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <terry/sampler/sampler.hpp>
#include <terry/point/operations.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

using namespace ::terry::sampler;
using boost::numeric_cast;

SeamCarvingPlugin::SeamCarvingPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_mapClip        = fetchClip ( kClipMap );
	_paramSize      = fetchInt2DParam ( kParamSize );
	_paramShowMap   = fetchBooleanParam ( kParamMap );
	_paramShowSeamCarving= fetchBooleanParam ( kParamSeamCarving );
}


SeamCarvingProcessParams<SeamCarvingPlugin::Scalar> SeamCarvingPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	SeamCarvingProcessParams<Scalar> params;

	OfxPointI size = _paramSize->getValue();
	params._outputSize.x = size.x;
	params._outputSize.y = size.y;

	params._showMap = _paramShowMap->getValue();
	params._showSeamCarving = _paramShowSeamCarving->getValue();

	return params;
}

void SeamCarvingPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
        clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
        clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
        clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 ); /// @todo tuttle: retrieve info from exr
}

void SeamCarvingPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{

}

bool SeamCarvingPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace boost::gil;

	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	const Point2   srcRodSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );

	const OfxPointI s = _paramSize->getValue();
	rod.x1 = 0;
	rod.y1 = 0;
	rod.x2 = s.x;
	rod.y2 = s.y;

	rod.x2 = srcRodSize.x;
	rod.y2 = srcRodSize.y;

	return true;

}

void SeamCarvingPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{

}

bool SeamCarvingPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	SeamCarvingProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

template< template<class, class> class Process, class MapLayout, class BitDepth, class DstLayout, class Plugin >
void SeamCarvingPlugin::renderMapComponentBitDepthDstComponent( Plugin& plugin, const OFX::RenderArguments& args )
{
    typedef boost::gil::pixel<BitDepth, MapLayout> MapPixel;
    typedef boost::gil::image<MapPixel, false> MapImage;
    typedef typename MapImage::view_t MapView;

    typedef boost::gil::pixel<BitDepth, DstLayout> DPixel;
    typedef boost::gil::image<DPixel, false> DImage;
    typedef typename DImage::view_t DView;

    Process<MapView, DView> procObj( plugin );

    procObj.setupAndProcess( args );
}


template< template<class, class> class Process, class MapLayout, class BitDepth, class Plugin >
void SeamCarvingPlugin::renderMapComponentBitDepth( Plugin& plugin, const OFX::RenderArguments& args, OFX::EPixelComponent dstComponents )
{
    switch( dstComponents )
    {
            case OFX::ePixelComponentAlpha:
            {
                    renderMapComponentBitDepthDstComponent<Process, MapLayout, BitDepth, boost::gil::gray_layout_t>( plugin, args );
                    return;
            }
            case OFX::ePixelComponentRGB:
            {
                    renderMapComponentBitDepthDstComponent<Process, MapLayout, BitDepth, boost::gil::rgb_layout_t>( plugin, args );
                    return;
            }
            case OFX::ePixelComponentRGBA:
            {
                    renderMapComponentBitDepthDstComponent<Process, MapLayout, BitDepth, boost::gil::rgba_layout_t>( plugin, args );
                    return;
            }
            case OFX::ePixelComponentCustom:
            case OFX::ePixelComponentNone:
            {
                    BOOST_THROW_EXCEPTION( exception::Unsupported()
                            << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(dstComponents) + ") not supported by the plugin." );
            }
    }
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void SeamCarvingPlugin::render( const OFX::RenderArguments &args )
{
    OFX::EBitDepth       bitDepth          = _clipDst->getPixelDepth();

    OFX::EPixelComponent mapComponents        = _mapClip->getPixelComponents();
    
    OFX::EPixelComponent dstComponents        = _clipDst->getPixelComponents();

    switch( mapComponents )
    {
            case OFX::ePixelComponentAlpha:
            {
                switch( bitDepth )
                {
                    case OFX::eBitDepthUByte:
                    {
                        renderMapComponentBitDepth<SeamCarvingProcess, boost::gil::gray_layout_t, boost::gil::bits8>( *this, args, dstComponents );
                        return;
                    }
                    case OFX::eBitDepthUShort:
                    {
                        renderMapComponentBitDepth<SeamCarvingProcess, boost::gil::gray_layout_t, boost::gil::bits16>( *this, args, dstComponents );
                        return;
                    }
                    case OFX::eBitDepthFloat:
                    {
                        renderMapComponentBitDepth<SeamCarvingProcess, boost::gil::gray_layout_t, boost::gil::bits32f>( *this, args, dstComponents );
                        return;
                    }
                    case OFX::eBitDepthCustom:
                    case OFX::eBitDepthNone:
                    {
                        BOOST_THROW_EXCEPTION( exception::Unsupported()
                                               << exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
                    }
                }
                return;
            }
            case OFX::ePixelComponentRGBA:
            case OFX::ePixelComponentRGB:
            case OFX::ePixelComponentCustom:
            case OFX::ePixelComponentNone:
            {
                    BOOST_THROW_EXCEPTION( exception::Unsupported()
                            << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(mapComponents) + ") not supported by the plugin." );
            }
    }
    BOOST_THROW_EXCEPTION( exception::Unknown() );
}


}
}
}
