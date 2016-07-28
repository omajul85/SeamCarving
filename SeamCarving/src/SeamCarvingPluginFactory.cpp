#include "SeamCarvingPluginFactory.hpp"
#include "SeamCarvingPlugin.hpp"
#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/context/SamplerPluginFactory.hpp>

#include <limits>


namespace tuttle {
namespace plugin {
namespace seamcarving {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void SeamCarvingPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleSeamCarving", "SeamCarving", "Image Retargeting" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
        desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void SeamCarvingPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
        OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* mapClip = desc.defineClip( kClipMap );
        mapClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	mapClip->setSupportsTiles( kSupportTiles );

	OFX::Int2DParamDescriptor* size = desc.defineInt2DParam( kParamSize );
	size->setLabel( kParamSizeLabel );
	size->setDefault( 600, 400 );
	size->setRange( 1, 1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max() );

	OFX::BooleanParamDescriptor* showMap = desc.defineBooleanParam( kParamMap );
	showMap->setLabel( kParamMapLabel );
	showMap->setDefault( false );

	//switch d'affichage de la source :
	OFX::BooleanParamDescriptor* showSeamCarving = desc.defineBooleanParam( kParamSeamCarving );
	showSeamCarving->setLabel( kParamSeamCarvingLabel );
	showSeamCarving->setDefault( false );
	
	
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* SeamCarvingPluginFactory::createInstance( OfxImageEffectHandle handle,
							    OFX::EContext context )
{
	return new SeamCarvingPlugin( handle );
}

}
}
}

