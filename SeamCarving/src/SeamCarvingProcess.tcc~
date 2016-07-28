#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/geometry/affine.hpp>
//#include <gray.hpp>
#include<time.h>

namespace tuttle {
namespace plugin {
namespace seamcarving {

template<class MapView, class View>
SeamCarvingProcess<MapView, View>::SeamCarvingProcess( SeamCarvingPlugin &effect )
        : ImageGilFilterProcessor<View>( effect , eImageOrientationFromTopToBottom )
	, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class MapView, class View>
void SeamCarvingProcess<MapView, View>::setup( const OFX::RenderArguments& args )
{
        ImageGilFilterProcessor<View>::setup( args );
        _params = _plugin.getProcessParams( args.renderScale );

	// clip Map
        _mapImg = _plugin._mapClip->fetchImage( args.time );
        if( _mapImg == NULL )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
        if( _mapImg->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );

        _mapPixelRod = _plugin._mapClip->getPixelRod( args.time, args.renderScale );
        _mapView = tuttle::plugin::getGilView<MapView>( _mapImg, _mapPixelRod, eImageOrientationFromTopToBottom );
}

template<class MapView>
void ProcessVerticalCumulSum( MapView& map, boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, int widthProcess)
{
	using namespace terry;
	float currentP;
	float topLeft;
	float top;
	float topRight;
	float minValue;
	int dir;
	
	for(int x = 0; x < widthProcess; x++ )
	{
		viewCumulSum(x,0) = map(x,0);
	}
	for(int y = 1; y < map.height(); y++ )
	{
		//First Column
		currentP = get_color( map(0,y), gray_color_t() );
		top      = get_color( viewCumulSum(0  ,y-1), gray_color_t() );
		topRight = get_color( viewCumulSum(1,y-1), gray_color_t() );
			
		if( topRight < top )
		{
			minValue = topRight;
			dir = 1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(0,y) = currentP + minValue;
		viewDirMap(0,y) = dir;
		
		//Last Column
		currentP = get_color( map(widthProcess-1,y), gray_color_t() );
		top      = get_color( viewCumulSum(widthProcess-1  ,y-1), gray_color_t() );
		topLeft  = get_color( viewCumulSum(widthProcess-2,y-1), gray_color_t() );			
		if( topLeft < top )
		{
			minValue = topLeft;
			dir = -1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(widthProcess-1,y) = currentP + minValue;
		viewDirMap(widthProcess-1,y) = dir;
		
		//Other Column
		for(int x = 1; x < widthProcess-1; x++ )
		{
			currentP = get_color( map(x,y), gray_color_t() );
			topLeft  = get_color( viewCumulSum(x-1,y-1), gray_color_t() );
			top      = get_color( viewCumulSum(x  ,y-1), gray_color_t() );
			topRight = get_color( viewCumulSum(x+1,y-1), gray_color_t() );
			minValue = top;
			dir = 0;
		
			if( topLeft < minValue )
			{
				minValue = topLeft;
				dir = -1;
			}
			else if( topRight < minValue)
			{
				minValue = topRight;
				dir = 1;
			}
			viewCumulSum(x,y) = currentP + minValue;
			viewDirMap(x,y) = dir;	
		} 
	}
}

void ProcessFoundVerticalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, std::vector<int>& verticalSeamPosition, int widthProcess )
{
	using namespace terry;
	/* Find the last seam element. (pixel to remove in the last arrow of the image)*/
	float min = get_color( viewCumulSum(0, viewCumulSum.height()-1), gray_color_t() ) ;
	int index = 0;
	for(int x = 1; x < widthProcess; x++ )
	{
		if ( get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) < min )
		{
			min = get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) ;
			index = x;
		}
	}
	verticalSeamPosition.at(viewCumulSum.height()-1) = index;
	/* Find the other seam elements */
	for(int y = 0 ; y <= viewCumulSum.height()-2 ; y++ )
	{
		verticalSeamPosition.at(viewCumulSum.height()-2 - y) = verticalSeamPosition.at(viewCumulSum.height()- 1 - y) + get_color( viewDirMap( verticalSeamPosition.at(viewCumulSum.height()- 1 - y) ,viewCumulSum.height()- 1 - y ), gray_color_t() ) ;
	}
}

template<class View, class MapView>
void ProcessRemoveVerticalSeam( View& processingSrc, MapView& map, std::vector<int>& verticalSeamPosition)
{
	using namespace terry;
	for (int y = 0; y < processingSrc.height(); y++)
	{
		for (int x = verticalSeamPosition.at(y); x < processingSrc.width()-1; x++)
		{
			processingSrc(x,y) = processingSrc(x+1,y);
			map(x,y) = map(x+1,y);
		}
	}
// 	TUTTLE_COUT("The seam is removed");
}

template<class MapView>
void ProcessHorizontalCumulSum( MapView& map, boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, int heightProcess)
{
	using namespace terry;
	float currentP;
	float topUp;
	float top;
	float topDown;
	float minValue;
	int dir;
	
	for(int y = 0; y < heightProcess; y++ )
	{
		viewCumulSum(0,y) = map(0,y);
	}
	for(int x = 1; x < map.width(); x++ )
	{
		//First arrow
		currentP = get_color( map(x,0), gray_color_t() );
		top      = get_color( viewCumulSum(x-1,0), gray_color_t() );
		topDown	 = get_color( viewCumulSum(x-1,1), gray_color_t() );
			
		if( topDown < top )
		{
			minValue = topDown;
			dir = 1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(x,0) = currentP + minValue;
		viewDirMap(x,0) = dir;
		
		//Last arrow
		currentP = get_color( map(x,heightProcess-1), gray_color_t() );
		top      = get_color( viewCumulSum(x-1,heightProcess-1), gray_color_t() );
		topUp	 = get_color( viewCumulSum(x-1,heightProcess-2), gray_color_t() );			
		
		if( topUp < top )
		{
			minValue = topUp;
			dir = -1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(x,heightProcess-1) = currentP + minValue;
		viewDirMap(x,heightProcess-1) = dir;
		
		//Other arrow
		for(int y = 1; y < heightProcess-1; y++ )
		{
			currentP = get_color( map(x,y), gray_color_t() );
			topUp	 = get_color( viewCumulSum(x-1,y-1), gray_color_t() );
			top      = get_color( viewCumulSum(x-1,y), gray_color_t() );
			topDown	 = get_color( viewCumulSum(x-1,y+1), gray_color_t() );
			minValue = top;
			dir = 0;
		
			if( topUp < minValue )
			{
				minValue = topUp;
				dir = -1;
			}
			else if( topDown < minValue)
			{
				minValue = topDown;
				dir = 1;
			}
			viewCumulSum(x,y) = currentP + minValue;
			viewDirMap(x,y) = dir;	
		} 
	}
}

void ProcessFoundHorizontalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, std::vector<int>& horizontalSeamPosition, int heightProcess )
{
	using namespace terry;
	/* Find the last seam element. (pixel to remove in the last arrow of the image)*/
	float min = get_color( viewCumulSum(viewCumulSum.width()-1,0), gray_color_t() ) ;
	int index = 0;
	for(int y = 1; y < heightProcess; y++ )
	{
		if ( get_color( viewCumulSum(viewCumulSum.width()-1,y), gray_color_t() ) < min )
		{
			min = get_color( viewCumulSum(viewCumulSum.width()-1,y), gray_color_t() ) ;
			index = y;
		}
	}
	horizontalSeamPosition.at(viewCumulSum.width()-1) = index;
	
	/* Find the other seam elements */
	for(int x = 0 ; x <= viewCumulSum.width()-2 ; x++ )
	{
		horizontalSeamPosition.at(viewCumulSum.width()-2 - x) = horizontalSeamPosition.at(viewCumulSum.width()- 1 - x) + get_color( viewDirMap( viewCumulSum.width()- 1 - x, horizontalSeamPosition.at(viewCumulSum.width()- 1 - x)), gray_color_t() ) ;
	}
}

template<class View, class MapView>
void ProcessRemoveHorizontalSeam( View& processingSrc, MapView& map, std::vector<int>& horizontalSeamPosition)
{
	using namespace terry;
	for (int x = 0; x < processingSrc.width(); x++)
	{
		for (int y = horizontalSeamPosition.at(x); y < processingSrc.height()-1; y++)
		{
			processingSrc(x,y) = processingSrc(x,y+1);
			map(x,y) = map(x,y+1);
		}
	}
	
	//TUTTLE_COUT("The seam is removed");
}


/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class MapView, class View>
void SeamCarvingProcess<MapView, View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	
	time_t start,end;
	double dif;

	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowOutput.x2 - procWindowOutput.x1, procWindowOutput.y2 - procWindowOutput.y1 };
	
	/* Affichage de src et dst */
	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );

	
	/* Affichage de map --> gradient */
        MapView map = subimage_view( _mapView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	/* Affichage de CumulSum image */
	gray32f_image_t cumulSum ( map.width(), map.height(), 0 );
	gray32f_view_t viewCumulSum ( view(cumulSum) );
	
	/* Affichage de direction map */
	gray32f_image_t dirMap ( map.width(), map.height(), 0 );
	gray32f_view_t viewDirMap ( view( dirMap ) );
	
	/* Image temporaire pendant SeamCarving Process : */
	View processingSrc = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	
	std::vector<int> verticalSeamPosition( viewCumulSum.height() ); 	// vecteur seam vertical
	std::vector<int> horizontalSeamPosition( viewCumulSum.width() ); 	// vecteur seam horizontal

	
	int iteration_x = src.width() - _params._outputSize.x;
	int widthProcess = 0;
	int iteration_y = src.height() - _params._outputSize.y;
	int heightProcess = 0;
	
	
	//copy_and_convert_pixels( map, dst );
	
	if( _params._showSeamCarving )
	{
		TUTTLE_COUT("show Seam Carving");
		TUTTLE_COUT_VAR(iteration_x);
		TUTTLE_COUT_VAR(iteration_y);
		time (&start);
		
		if ( iteration_x > iteration_y )
		{
			int itDiffx = iteration_x - iteration_y;
			TUTTLE_COUT_VAR(itDiffx);
			for (int i = 0; i < iteration_y; i++)
			{
				//Vertical reduction
				widthProcess = src.width() - i;
				ProcessVerticalCumulSum( map, viewDirMap, viewCumulSum, widthProcess);
				ProcessFoundVerticalSeam( viewDirMap, viewCumulSum, verticalSeamPosition, widthProcess);
				ProcessRemoveVerticalSeam(processingSrc, map, verticalSeamPosition);
				
				//Horizontal reduction
				heightProcess = src.height() - i;
				ProcessHorizontalCumulSum( map, viewDirMap, viewCumulSum, heightProcess);
				ProcessFoundHorizontalSeam( viewDirMap, viewCumulSum, horizontalSeamPosition, heightProcess );
				ProcessRemoveHorizontalSeam(processingSrc, map, horizontalSeamPosition);
			}
			
			for (int i = iteration_y; i < iteration_x; i++)
			{
				//Vertical reduction
				widthProcess = src.width() - i;
				ProcessVerticalCumulSum( map, viewDirMap, viewCumulSum, widthProcess);
				ProcessFoundVerticalSeam( viewDirMap, viewCumulSum, verticalSeamPosition, widthProcess);
				ProcessRemoveVerticalSeam(processingSrc, map, verticalSeamPosition);
			}
		}
		
		
		if ( iteration_y > iteration_x )
		{
			int itDiffy = iteration_y - iteration_x;
			TUTTLE_COUT_VAR(itDiffy);
			for (int i = 0; i < iteration_x; i++)
			{
				//Horizontal reduction
				heightProcess = src.height() - i;
				ProcessHorizontalCumulSum( map, viewDirMap, viewCumulSum, heightProcess);
				ProcessFoundHorizontalSeam( viewDirMap, viewCumulSum, horizontalSeamPosition, heightProcess );
				ProcessRemoveHorizontalSeam(processingSrc, map, horizontalSeamPosition);
				
				//Vertical reduction
				widthProcess = src.width() - i;
				ProcessVerticalCumulSum( map, viewDirMap, viewCumulSum, widthProcess);
				ProcessFoundVerticalSeam( viewDirMap, viewCumulSum, verticalSeamPosition, widthProcess);
				ProcessRemoveVerticalSeam(processingSrc, map, verticalSeamPosition);
			}
			
			for (int i = iteration_x; i < iteration_y; i++)
			{
				//Horizontal reduction
				heightProcess = src.height() - i;
				ProcessHorizontalCumulSum( map, viewDirMap, viewCumulSum, heightProcess);
				ProcessFoundHorizontalSeam( viewDirMap, viewCumulSum, horizontalSeamPosition, heightProcess );
				ProcessRemoveHorizontalSeam(processingSrc, map, horizontalSeamPosition);
			}
		}
		
		if ( iteration_y == iteration_x )
		{
			for (int i = 0; i < iteration_x; i++)
			{
				//Vertical reduction
				widthProcess = src.width() - i;
				ProcessVerticalCumulSum( map, viewDirMap, viewCumulSum, widthProcess);
				ProcessFoundVerticalSeam( viewDirMap, viewCumulSum, verticalSeamPosition, widthProcess);
				ProcessRemoveVerticalSeam(processingSrc, map, verticalSeamPosition);

				//Horizontal reduction
				heightProcess = src.height() - i;
				ProcessHorizontalCumulSum( map, viewDirMap, viewCumulSum, heightProcess);
				ProcessFoundHorizontalSeam( viewDirMap, viewCumulSum, horizontalSeamPosition, heightProcess );
				ProcessRemoveHorizontalSeam(processingSrc, map, horizontalSeamPosition);
			}
		}
		
		copy_and_convert_pixels( processingSrc, dst);
		
		time (&end);
		dif = difftime (end,start);
		TUTTLE_COUT_VAR(dif);
		
        }
        else if( _params._showMap )
        {
		copy_and_convert_pixels( map, dst );
		TUTTLE_COUT("showMap active");
	}
	else if( !_params._showSeamCarving )
	{
		copy_and_convert_pixels( src, dst );
	}
}

}
}
}
