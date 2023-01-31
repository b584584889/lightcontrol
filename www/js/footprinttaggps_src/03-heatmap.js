/*
 * Author: ikki Chung
 * Date: 2019 07 17
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

// https://github.com/Leaflet/Leaflet.heat
//

function HeatMap(target_mapid)
{
    // Coordinates in CRS.Simple take the form of [y, x] instead of [x, y]
    //
    var yx = L.latLng;

    var xy = function(x, y) {
        if (L.Util.isArray(x)) {    // When doing xy([x, y]);
            return yx(x[1], x[0]);
        }
        return yx(y, x);  // When doing xy(x, y);
    };

    var mWGS48OriginX = 0;
    var mWGS48OriginY = 0;
    var mMAP_North = 0;
    var mUsermap_width = 0;
    var mUsermap_height = 0;
    var mMap_scale = 0;
    var mMap_coeff = 0;
    var mMap_OrigX = 0;
    var mMap_OrigY = 0;
    var mBounds = [xy(0, 0), xy(1000, 1000)];

    var mUserMaplayer = null;
    var mUserMap_url = null;
    var mUserProject_Name = null;

    var mRotateMap = false;

    var mMap = L.map(target_mapid).setView([-37.82109, 175.2193], 16);
    mMap.attributionControl.setPrefix('<a href="http://smims.com">SMIMS</a>')

    // var tiles = L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {
    //     attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors',
    // }).addTo(map);

    var gmapStreets = L.tileLayer('http://{s}.google.com/vt/lyrs=m&x={x}&y={y}&z={z}', {
        attribution: 'Google',
        maxZoom: 26,
        subdomains:['mt0','mt1','mt2','mt3']
    }).addTo(mMap);

    //var m_addressPoints = addressPoints.map(function (p) { return [p[0], p[1]]; });
    // var m_addressPoints = [];

    // var mHeatMap = null;
    // var mDraw = false;
    var mHeatMap = L.heatLayer([], {radius:25, max:1, blur:10, gradient:{0.4: 'blue', 0.65: 'lime', 1: 'red'}}).addTo(mMap);
    var mDraw = true;

    var mTagPathLayer = new L.LayerGroup();
    var mTagPointLayer = new L.LayerGroup();

    // var mPointLine = new L.Polyline([], {
    // color: '#FF6347',
    // weight: 4,
    // opacity: 0.8,
    // smoothFactor: 1
    // }).addTo(mMap);

    // var mPointLine = new L.Polyline([], {
    // color: '#FF6347',
    // weight: 4,
    // opacity: 0.8,
    // smoothFactor: 1
    // });

    var mLayerControl = L.control.layers().addTo(mMap);
    mLayerControl.addOverlay(gmapStreets, "Google Map");
    mLayerControl.addOverlay(mHeatMap, "Heat Map");
    mLayerControl.addOverlay(mTagPathLayer, "軌跡圖");
    mLayerControl.addOverlay(mTagPointLayer, "定位點");

    // mTagPathLayer.addLayer(mPointLine);
    mMap.addLayer(mTagPathLayer, "軌跡圖");
    mMap.addLayer(mTagPointLayer, "定位點");

    mMap.on({
        movestart: function () { mDraw = false; },
        moveend:   function () { mDraw = true; },
        mousemove: function (e) {
            // if (draw) {
            //     heat.addLatLng(e.latlng);
            // }
        }
    });

    this.setMapCoeff = function(map_origX, map_origY, width, height, scale, coeff, WGS48OriginX, WGS48OriginY, MAP_North)
    {
        if (map_origX == null )
            map_origX = 0.0;
        if (map_origY == null )
            map_origY = 0.0;
        mWGS48OriginX = parseFloat(WGS48OriginX);
        mWGS48OriginY = parseFloat(WGS48OriginY);
        mMAP_North = parseFloat(MAP_North);
        mUsermap_width = parseInt(width);
        mUsermap_height = parseInt(height);
        mMap_scale = parseFloat(scale);
        mMap_coeff = parseFloat(coeff);
        mMap_OrigX = parseFloat(map_origX);
        mMap_OrigY = parseFloat(map_origY);
    }

    this.setMapImage = function(usr_map_url, project_name)
    {
        mUserMap_url = usr_map_url;
        mUserProject_Name = project_name;

        update_2d_map();

        mMap.fitBounds(mBounds, {paddingTopLeft: [0,0]});
    }

    this.addRecords = function(records)
    {
        if (mDraw)
        {
            var i=0;
            for(i in records)
            {
                if ( i == 0)
                {
                    mMap.setView(records[i], 16);
                }

                mHeatMap.addLatLng(records[i]);
                // mPointLine.addLatLng(records[i]);
            }
        }
    }

    this.addLineRecords = function(records, datetimes)
    {
        if (mDraw)
        {
            var mPointLine = new L.Polyline([], {
            color: '#FF6347',
            weight: 4,
            opacity: 0.8,
            smoothFactor: 1
            });

            mTagPathLayer.addLayer(mPointLine);

            var i=0;
            for(i in records)
            {
                //var popName = records[i] +"&nbsp;<br>" + datetimes[i];
                var popName = datetimes[i];

                var markpopup = L.popup({
                  closeOnClick: false,
                  autoClose: false
                }).setContent(popName);

                const latlngs = L.circleMarker(records[i],{radius: 5, opacity: .9,}).addTo(mMap).bindPopup(markpopup);
                mTagPointLayer.addLayer(latlngs);

                mPointLine.addLatLng(records[i]);
            }
        }
    }

    this.addDashLine = function(records, datetimes)
    {
        if (mDraw)
        {
            var mPointLine = new L.Polyline([], {
            color: '#FF6347',
            weight: 4,
            opacity: 0.8,
            smoothFactor: 1
            });

            mPointLine.setStyle({
                color: 'black',
                dashArray: '5, 10',
                lineCap: 'square', // Optional, just to avoid round borders.
            });

            // var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, maxWidth: "190"})
            //           .setContent('addDashLine');
            // mPointLine.bindPopup(markerpopup);

            mTagPathLayer.addLayer(mPointLine);

            var i=0;
            for(i in records)
            {
                //var popName = records[i] +"&nbsp;<br>" + datetimes[i];
                var popName = datetimes[i];

                var markpopup = L.popup({
                  closeOnClick: false,
                  autoClose: false
                }).setContent(popName);

                const latlngs = L.circleMarker(records[i],{radius: 5, opacity: .9,}).addTo(mMap).bindPopup(markpopup);
                mTagPointLayer.addLayer(latlngs);

                mPointLine.addLatLng(records[i]);
            }
        }
    }

    this.addWarningDashLine = function(records, datetimes)
    {
        if (mDraw)
        {
            var mPointLine = new L.Polyline([], {
            color: '#FF6347',
            weight: 4,
            opacity: 0.8,
            smoothFactor: 1
            });

            mPointLine.setStyle({
                color: '#FF6347',
                dashArray: '5, 10',
                lineCap: 'square', // Optional, just to avoid round borders.
            });

            // var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, maxWidth: "190"})
            //           .setContent('addDashLine');
            // mPointLine.bindPopup(markerpopup);

            mTagPathLayer.addLayer(mPointLine);

            var i=0;
            for(i in records)
            {
                //var popName = records[i] +"&nbsp;<br>" + datetimes[i];
                var popName = datetimes[i];

                var markpopup = L.popup({
                  closeOnClick: false,
                  autoClose: false
                }).setContent(popName);

                const latlngs = L.circleMarker(records[i],{radius: 5, opacity: .9,}).addTo(mMap).bindPopup(markpopup);
                mTagPointLayer.addLayer(latlngs);

                mPointLine.addLatLng(records[i]);
            }
        }
    }

    this.refresh = function(records)
    {
        mMap._onResize();
    }

    function update_2d_map()
    {
        if (mUserMaplayer != null)
		      mMap.removeLayer(mUserMaplayer);

        var mapScaleWidth = FloatDiv(mUsermap_width, mMap_scale * mMap_coeff);
        var mapScaleHeight = FloatDiv(mUsermap_height, mMap_scale * mMap_coeff);

        var NodeLatitude = mWGS48OriginY + centimeterToLatitude(FloatMul(mMap_OrigY, -1.0));
        var NodeLongitude = mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(mMap_OrigX, -1.0));
        var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);

        var mapRightTopLatLng = xy(mapLeftBottomLatLng.lng + mapScaleWidth, mapLeftBottomLatLng.lat + mapScaleHeight);

        mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];

        var pointTopLeft = new L.latLng(mBounds[1].lat, mBounds[0].lng);
        var pointTopRight = new L.latLng(mBounds[1].lat, mBounds[1].lng);
        var pointBottomLeft = new L.latLng(mBounds[0].lat, mBounds[0].lng);
        var pointOrigin = new L.latLng(mWGS48OriginY, mWGS48OriginX);

        var rotateAngle = 0;
        if (mRotateMap)
        {
            rotateAngle = mMAP_North;
        }

        var rotatePointTopLeft = rotatePoint(pointTopLeft, rotateAngle, pointOrigin);
        var rotatePointTopRight = rotatePoint(pointTopRight, rotateAngle, pointOrigin);
        var rotatePointBottomLeft = rotatePoint(pointBottomLeft, rotateAngle, pointOrigin);

        mUserMaplayer = L.imageOverlay.rotated(mUserMap_url, rotatePointTopLeft, rotatePointTopRight, rotatePointBottomLeft, {
        		opacity: 0.7,
        		interactive: true,
              attribution: mUserProject_Name,
        	});
        mMap.addLayer(mUserMaplayer, mUserProject_Name);

    }

}
