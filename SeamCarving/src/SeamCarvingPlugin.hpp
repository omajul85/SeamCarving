#ifndef _TUTTLE_PLUGIN_SEAMCARVING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SEAMCARVING_PLUGIN_HPP_

#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/context/SamplerPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

template<typename Scalar>
struct SeamCarvingProcessParams
{
	OFX::Clip* _clipMap;
	
	boost::gil::point2<Scalar> _outputSize;
	bool                       _showMap;
	bool                       _showSeamCarving;
};

/**
 * @brief SeamCarving plugin
 */
class SeamCarvingPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;

public:
	SeamCarvingPlugin( OfxImageEffectHandle handle );

public:
	SeamCarvingProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;


	void changedParam          ( const OFX::InstanceChangedArgs &args, const std::string &paramName );

        void getClipPreferences    ( OFX::ClipPreferencesSetter& clipPreferences );
	bool getRegionOfDefinition ( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest  ( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity            ( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render                ( const OFX::RenderArguments &args );

private:

        template< template<class, class> class Process, class MapLayout, class BitDepth, class DstLayout, class Plugin >
        void renderMapComponentBitDepthDstComponent( Plugin& plugin, const OFX::RenderArguments& args);

        template< template<class, class> class Process, class MapLayout, class BitDepth, class Plugin >
        void renderMapComponentBitDepth( Plugin& plugin, const OFX::RenderArguments& args, OFX::EPixelComponent dstComponents );
public:
	OFX::Clip* _mapClip;
	
	OFX::Int2DParam*    _paramSize;
	OFX::BooleanParam*  _paramShowMap;
	OFX::BooleanParam*  _paramShowSeamCarving;
};

}
}
}

#endif
