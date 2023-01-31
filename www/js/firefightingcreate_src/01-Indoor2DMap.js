/*
 * Author: ikki Chung
 * Date: 19 Jan 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var Indoor2DMap = function(mapid)
{
    this.mEnableUISmoothing = true;
    this.SHOW_AREA_DEFAULT = false;
    this.SHOW_LOCATOR_DEFAULT = false;

    //
    // Coordinates in CRS.Simple take the form of [y, x] instead of [x, y]
    //
    //var yx = L.latLng;

    var xy = function(x, y)
    {
        if (L.Util.isArray(x))
        {    // When doing xy([x, y]);
            return L.latLng(x[1], x[0]);
        }
        return L.latLng(y, x);  // When doing xy(x, y);
    };

	/*
	* Icons
	*/
	var origIcon = L.icon({
	    iconUrl: '/images/orig-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var coordIcon = L.icon({
	    iconUrl: '/images/coord-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var tagIcon = L.icon({
	    iconUrl: '/images/marker-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var tagLowBatteryIcon = L.icon({
	    iconUrl: '/images/tag-lowbattery-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var tagSleepIcon = L.icon({
	    iconUrl: '/images/marker-sleep-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var tagSleepLowBatteryIcon = L.icon({
	    iconUrl: '/images/marker-sleep-lowbattery-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var anchorIcon = L.icon({
	    iconUrl: '/images/anchor-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var locatorIcon = L.icon({
	    iconUrl: '/images/locator-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var locatornotaliveIcon = L.icon({
	    iconUrl: '/images/locator-notalive-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

        iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var locatorPowerOffIcon = L.icon({
	    iconUrl: '/images/locator-poweroff-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

        iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var grayIcon = L.icon({
	    iconUrl: '/images/gray-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var grayLowBatteryIcon = L.icon({
	    iconUrl: '/images/gray-lowbattery-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var alertIcon = L.icon({
	    iconUrl: '/images/alert-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var alert2xIcon = L.icon({
	    iconUrl: '/images/alert-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [50, 74], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [25, 74], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var inactiveIcon = L.icon({
	    iconUrl: '/images/inactive-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var inactiveLowBatteryIcon = L.icon({
	    iconUrl: '/images/inactive-lowbattery-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

	var norollcallIcon = L.icon({
	    iconUrl: '/images/norollcall-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var norollcallLowBatteryIcon = L.icon({
	    iconUrl: '/images/norollcall-lowbattery-icon-2x.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [30, 47], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

    var smartWatchIcon = L.icon({
	    iconUrl: '/images/pin_watch.png',
        // iconUrl: '/images/smartwatchcolor.png',
        shadowUrl: '/images/marker-shadow.png',

        iconSize:     [42, 56], // size of the icon
        shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [21, 56], // point of the icon which will correspond to marker's location
	    shadowAnchor: [15, 56],  // the same for the shadow
	    popupAnchor:  [0, -56] // point from which the popup should open relative to the iconAnchor
	});

    var smartWatchLowBatteryIcon = L.icon({
        iconUrl: '/images/pin_watch.png',
        //iconUrl: '/images/smartwatchcolor_lowbattry.png',
        shadowUrl: '/images/marker-shadow.png',

        iconSize:     [42, 56], // size of the icon
        shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [21, 56], // point of the icon which will correspond to marker's location
	    shadowAnchor: [15, 56],  // the same for the shadow
	    popupAnchor:  [0, -56] // point from which the popup should open relative to the iconAnchor
	});

    var smartWatchUnknownLocationIcon = L.icon({
        iconUrl: '/images/pin_watch.png',
        shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [42, 56], // size of the icon
        shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [21, 56], // point of the icon which will correspond to marker's location
	    shadowAnchor: [15, 56],  // the same for the shadow
	    popupAnchor:  [0, -56] // point from which the popup should open relative to the iconAnchor
	});

    // var smartWatchIcon = L.icon({
	//     iconUrl: '/images/pin_watch.png',
    //     // iconUrl: '/images/smartwatchcolor.png',
    //     shadowUrl: '/images/marker-shadow.png',
    //
	//     iconSize:     [30, 47], // size of the icon
	//     shadowSize:   [41, 41], // size of the shadow
	//     iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	//     shadowAnchor: [14, 41],  // the same for the shadow
	//     popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	// });

    // var smartWatchLowBatteryIcon = L.icon({
    //     iconUrl: '/images/pin_watch.png',
    //     //iconUrl: '/images/smartwatchcolor_lowbattry.png',
    //     shadowUrl: '/images/marker-shadow.png',
    //
	//     iconSize:     [30, 47], // size of the icon
	//     shadowSize:   [41, 41], // size of the shadow
	//     iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
	//     shadowAnchor: [14, 41],  // the same for the shadow
	//     popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	// });

    // var smartWatchUnknownLocationIcon = L.icon({
    //     iconUrl: '/images/pin_watch.png',
    //     //iconUrl: '/images/smartwatchcolor_unknown.png',
    //     shadowUrl: '/images/marker-shadow.png',
    //
    //     iconSize:     [30, 47], // size of the icon
    //     shadowSize:   [41, 41], // size of the shadow
    //     iconAnchor:   [15, 47], // point of the icon which will correspond to marker's location
    //     shadowAnchor: [14, 41],  // the same for the shadow
    //     popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
    // });

    var LiDARIcon = L.icon({
	    iconUrl: '/images/pin_lidar.png',
        shadowUrl: '/images/marker-shadow.png',

        iconSize:     [42, 56], // size of the icon
        shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [21, 56], // point of the icon which will correspond to marker's location
	    shadowAnchor: [15, 56],  // the same for the shadow
	    popupAnchor:  [0, -56] // point from which the popup should open relative to the iconAnchor
	});

    var UserIcon = L.icon({
	    iconUrl: '/images/pin_user.png',
        shadowUrl: '/images/marker-shadow.png',

        iconSize:     [42, 56], // size of the icon
        shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [21, 56], // point of the icon which will correspond to marker's location
	    shadowAnchor: [15, 56],  // the same for the shadow
	    popupAnchor:  [0, -56] // point from which the popup should open relative to the iconAnchor
	});

    // var LiDARIcon = L.icon({
	//     iconUrl: '/images/lidar.png',
    //     shadowUrl: '/images/marker-shadow.png',
    //
	//     iconSize:     [47, 47], // size of the icon
	//     shadowSize:   [41, 41], // size of the shadow
	//     iconAnchor:   [23, 40], // point of the icon which will correspond to marker's location
	//     shadowAnchor: [14, 41],  // the same for the shadow
	//     popupAnchor:  [0, -25] // point from which the popup should open relative to the iconAnchor
	// });

    // var UserIcon = L.icon({
	//     iconUrl: '/images/usermap.png',
    //     shadowUrl: '/images/marker-shadow.png',
    //
	//     iconSize:     [47, 47], // size of the icon
	//     shadowSize:   [41, 41], // size of the shadow
	//     iconAnchor:   [23, 40], // point of the icon which will correspond to marker's location
	//     shadowAnchor: [14, 41],  // the same for the shadow
	//     popupAnchor:  [0, -25] // point from which the popup should open relative to the iconAnchor
	// });

    //https://www.w3schools.com/cssref/css_colors.asp
    var footprintcolors = [
        '#FF6347', // Tomato
        '#FFA500', // Orange
        '#FFD700', // Gold
        '#32CD32', // LimeGreen
        '#87CEEB', // SkyBlue
        '#7B68EE', // MediumSlateBlue
        '#8A2BE2', // BlueViolet
        '#4245f4', // Blue
        '#F08080', // LightCoral
        '#f441d9'  // pink
    ];

	/*
	* Layers
	*/
    this.mMapLayer_nodes = [];
    this.mMapLayerId = 1;

    this.OriginMarker = null;
	this.Anchors = [];
	this.Tags = [];
    this.Coords = [];
    this.Locators = [];
    this.Sensors = [];
    this.Users = [];
    this.TagDrawList = [];
    this.TagGPSDrawList = [];
    this.MapAreas = [];
    this.AddressList = [];

    this.AlertIconScaleIndexList = [];
    this.AlertIconScaleUpDownList = [];

    this.StreetMaps = [[]];

    this.NodeAlertFunction = [];// 0:disable , 1:enable

    // map area border
    this.areaborder_color = '#8b95aa';
    this.areaborder_opacity = '0.8';

    this.FadeOutPolylines = [[]];
    this.FadeOutNodes = [[]];
    //var TagPredictRanging = [];
    this.mShowTrack = false;
    this.mRotateMap = false;

    this.mNodeDragCallback = null;
    this.mOrigNodeDragCallback = null;
    this.mUpdateMapInfoCallback = null;
    this.mNodeClickCallback = null;
    this.mEditModeCallback = null;
    this.mTargetDragCallback = null;

    //
	// Add Control
	//
    this.mEdit_mode = false;
    this.mEdit_delete_mode = false;
 	this.mEdit_maparea_index = -1;
	this.mUsermapEdit = null;
    this.mUsermapScaleUp = null;
    this.mUsermapScaleDown = null;
    this.mLayerControl = null;

    this.mWGS48OriginX = 0;
    this.mWGS48OriginY = 0;
    this.mMAP_North = 0;
    this.mUsermap_width = 0;
    this.mUsermap_height = 0;
    this.mMap_scale = 0;
    this.mMap_coeff = 0;
    this.mMap_OrigX = 0;
    this.mMap_OrigY = 0;
    this.mBounds = [xy(0, 0), xy(1000, 1000)];

    this.mUserMaplayer = null;
    this.mUserMap_url = null;
    this.mUserProject_Name = null;

    this.smoothing_time = 400;// ms
	this.move_step_size = 6;
	this.move_loop_count = 0;

    this.GPS_smoothing_time = 400;// ms
	this.GPS_move_step_size = 10.0;
	this.GPS_move_loop_count = 0;

    this.popupZoom = 20;

    this.mDrawControl = null;

	this.baseLayers = {
		//"Diamond": Diamondlayer
	};

	// Leaflet.draw
	// Initialize the FeatureGroup to store editable layers
	this.layer_areas = new L.FeatureGroup();

	this.overlays = {
		//"Coordinators": layer_coords,
		//"Anchors": layer_anchors,
		//"Tags": layer_tags,
        //"Areas": layer_areas
	};
	//var map_center = xy(0.00022, 0.00049);

    //
    // keep popup open
    //
    L.Map = L.Map.extend({
     	openPopup: function (popup, latlng, options)
        {
            if (!(popup instanceof L.Popup))
            {
                var content = popup;
                popup = new L.Popup(options).setContent(content);
            }

            if (latlng) {
            popup.setLatLng(latlng);
            }

            if (this.hasLayer(popup)) {
            return this;
            }

            //this.closePopup();
            this._popup = popup;
            return this.addLayer(popup);
        }
    });

    this.mMap = L.map(mapid, {
		//crs: L.CRS.Simple,
        // minZoom: 15,
		maxZoom: 26,
		// zoomControl: true,
        zoomSnap: 0,
		zoomDelta: 0.25,
		// wheelPxPerZoomLevel: 50,
		doubleClickZoom: false,
        // center: new L.LatLng(51.505, -0.04)
		//layers: [layer_areas]
        //layers: [layer_coords, layer_anchors, layer_tags, layer_areas]
	});
	//mMap.addLayer(UserMaplayer, project_name);

    this.mLayerControl = L.control.layers().addTo(this.mMap);


    //mMap.attributionControl.addAttribution('Licensed by &copy; <a href="some_link", class="your_class">Habijabi</a>')
    this.mMap.attributionControl.setPrefix('<a href="http://smims.com">SMIMS</a>')
    this.mMap.orig_this = this;
    //mMap.attributionControl.addAttribution('<a href="http://smims.com">VILS</a>');

    //
    // OpenStreetMap
    //
    // L.tileLayer('https://api.tiles.mapbox.com/v4/{id}/{z}/{x}/{y}.png?access_token={accessToken}', {
    //     attribution: 'SMIMS Inc,',
    //     maxZoom: 18,
    //     id: 'mapbox.streets',
    //     accessToken: 'pk.eyJ1IjoiaWtraXNtaW1zIiwiYSI6ImNqcHl5MGs5bzA2OXEzeG9kMDB0bnc1ZmwifQ.IM46UIqIFAzmlEO0lUw4cA'
    // }).addTo(mMap);
    var osmUrl = 'http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png';
    var osmAttrib = '&copy; <a href="http://openstreetmap.org/copyright">OpenStreetMap</a> contributors';

    var osm = L.tileLayer(osmUrl, { maxZoom: 26, attribution: osmAttrib });
    //map = new L.Map('map', { center: new L.LatLng(51.505, -0.04), zoom: 13 }),
    //drawnItems = L.featureGroup().addTo(map);
    var gmapStreets = L.tileLayer('http://{s}.google.com/vt/lyrs=m&x={x}&y={y}&z={z}', {
        attribution: 'Google',
        maxZoom: 26,
        subdomains:['mt0','mt1','mt2','mt3']
    });
    var gmapSat = L.tileLayer('http://{s}.google.com/vt/lyrs=s&x={x}&y={y}&z={z}', {
        attribution: 'Google',
        maxZoom: 26,
        subdomains:['mt0','mt1','mt2','mt3']
    });
    // var gmapTerrain = L.tileLayer('http://{s}.google.com/vt/lyrs=p&x={x}&y={y}&z={z}', {
    //     attribution: 'Google',
    //     maxZoom: 26,
    //     subdomains:['mt0','mt1','mt2','mt3']
    // });


    this.mLayerControl.addOverlay(osm, "OpenStreetMap");
    this.mLayerControl.addOverlay(gmapStreets, "Google Streets");
    this.mLayerControl.addOverlay(gmapSat, "Google Satellite");

    this.StreetMaps["OpenStreetMap"] = 0; // 0:not Show 1:on map
    this.StreetMaps["Google Streets"] = 0; // 0:not Show  1:on map
    this.StreetMaps["Google Satellite"] = 0; // 0:not Show  1:on map

    Indoor2DMap.prototype.create_alert_scale_icon = function()
    {
        var i;
        for(i=0;i<10;i++)
        {
            var newWidth = 25*(1+ 0.1*i);
            var newHieght = 37*(1+ 0.1*i);

            var newAlertIcon = L.icon({
        	    iconUrl: '/images/alert-icon-2x.png',
        	    shadowUrl: '/images/marker-shadow.png',

        	    iconSize:     [newWidth, newHieght], // size of the icon
        	    shadowSize:   [41, 41], // size of the shadow
        	    iconAnchor:   [newWidth/2, newHieght], // point of the icon which will correspond to marker's location
        	    shadowAnchor: [14, 41],  // the same for the shadow
        	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
        	});

            this.AlertIconScaleUpDownList[i] = newAlertIcon;
        }
    };

    Indoor2DMap.prototype.onOverlayAdd = function(e)
    {
        //Note that the this keyword in the event listener is the map object i.e. this is the same as map.
        //console.log("onOverlayAdd() e.name:"+e.name);
        var cur_map = this;
        if ( e.name in cur_map.orig_this.StreetMaps )
        {
            cur_map.orig_this.StreetMaps[e.name] = 1;
            cur_map.orig_this.updateMapRotate(cur_map.orig_this);
        }

    };

    Indoor2DMap.prototype.onOverlayRemove = function(e)
    {
        //console.log("onOverlayRemove() e.name:"+e.name);
        var cur_map = this;
        if ( e.name in cur_map.orig_this.StreetMaps )
        {
            cur_map.orig_this.StreetMaps[e.name] = 0;
            cur_map.orig_this.updateMapRotate(cur_map.orig_this);
        }

    };

    Indoor2DMap.prototype.updateMapRotate = function(orig_this)
    {
        //console.log("updateMapRotate() StreetMaps:"+StreetMaps);

        var NeedRotate = false;
        for( map in orig_this.StreetMaps)
        {
            var value = orig_this.StreetMaps[map];
            //console.log("updateMapRotate() map:"+map+", value:"+value);
            if (value == 1)
            {
                NeedRotate = true;
                break;
            }
        }
        orig_this.mRotateMap = NeedRotate;

        orig_this.update_2d_map(orig_this);
        orig_this.update_node_pos(orig_this, orig_this.Anchors);
        orig_this.update_node_pos(orig_this, orig_this.Coords);
        orig_this.load_allmaparea(orig_this);
    };

    Indoor2DMap.prototype.update_2d_map = function(orig_this)
    {
        if (orig_this.mUserMaplayer != null)
		      orig_this.mMap.removeLayer(orig_this.mUserMaplayer);

        var mapScaleWidth = FloatDiv(orig_this.mUsermap_width, orig_this.mMap_scale * orig_this.mMap_coeff);
        var mapScaleHeight = FloatDiv(orig_this.mUsermap_height, orig_this.mMap_scale * orig_this.mMap_coeff);

        //console.log("update_2d_map() mapScaleWidth:"+mapScaleWidth +", mapScaleHeight:"+mapScaleHeight);

        //=====================
        //var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
        //=====================
        var NodeLatitude = orig_this.mWGS48OriginY + centimeterToLatitude(FloatMul(orig_this.mMap_OrigY, -1.0));
        var NodeLongitude = orig_this.mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(orig_this.mMap_OrigX, -1.0));
        var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);

        var TenMeterLatitude = centimeterToLatitude(10000);
        var TenMeterLongitude = centimeterToLongitude(orig_this.mWGS48OriginY + TenMeterLatitude, 10000);
        var ratio = TenMeterLatitude / TenMeterLongitude;
        //console.log("update_2d_map() TenMeterLatitude:"+TenMeterLatitude +", TenMeterLongitude:"+TenMeterLongitude +", ratio:"+ratio);
        var mapScaleHeight = mapScaleHeight * ratio;
        //=====================
        //var NodeLatitude = mWGS48OriginY + centimeterToLatitude(FloatMul(mMap_OrigY, -1.0));
        //var NodeLongitude = mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(mMap_OrigX, -1.0));
        //var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);
        //=====================

        var mapRightTopLatLng = xy(mapLeftBottomLatLng.lng + mapScaleWidth, mapLeftBottomLatLng.lat + mapScaleHeight);

        orig_this.mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];

        // console.log("update_2d_map() this.mBounds:"+this.mBounds);

        var pointTopLeft = new L.latLng(orig_this.mBounds[1].lat, orig_this.mBounds[0].lng);
        var pointTopRight = new L.latLng(orig_this.mBounds[1].lat, orig_this.mBounds[1].lng);
        var pointBottomLeft = new L.latLng(orig_this.mBounds[0].lat, orig_this.mBounds[0].lng);
        var pointOrigin = new L.latLng(orig_this.mWGS48OriginY, this.mWGS48OriginX);
        //var pointOrigin = convertToLatLng(mMap_OrigX, mMap_OrigY);

        // var rotatePointTopLeft = rotatePoint(pointTopLeft, mMAP_North, pointOrigin);
        // var rotatePointTopRight = rotatePoint(pointTopRight, mMAP_North, pointOrigin);
        // var rotatePointBottomLeft = rotatePoint(pointBottomLeft, mMAP_North, pointOrigin);
        var rotateAngle = 0;
        if (orig_this.mRotateMap)
        {
            rotateAngle = orig_this.mMAP_North;
        }

        var rotatePointTopLeft = rotatePoint(pointTopLeft, rotateAngle, pointOrigin);
        var rotatePointTopRight = rotatePoint(pointTopRight, rotateAngle, pointOrigin);
        var rotatePointBottomLeft = rotatePoint(pointBottomLeft, rotateAngle, pointOrigin);

        // if (rotatePointTopLeft.lat == rotatePointBottomLeft.lat)
        // {
        //     rotatePointTopLeft.lat = rotatePointTopLeft.lat + 0.000001;
        // }
        // if (rotatePointTopLeft.lng == rotatePointTopRight.lng)
        // {
        //     rotatePointTopLeft.lng = rotatePointTopLeft.lng + 0.000001;
        // }

        orig_this.mUserMaplayer = L.imageOverlay.rotated(orig_this.mUserMap_url, rotatePointTopLeft, rotatePointTopRight, rotatePointBottomLeft, {
        		opacity: 0.5,
        		interactive: true,
              attribution: orig_this.mUserProject_Name,
        	});
        orig_this.mMap.addLayer(orig_this.mUserMaplayer, orig_this.mUserProject_Name);

        //=======================
        // var mapScaleWidth = FloatDiv(mUsermap_width, mMap_scale * mMap_coeff);
        // var mapScaleHeight = FloatDiv(mUsermap_height, mMap_scale * mMap_coeff);
        // var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
        // var mapRightTopLatLng = xy(mapLeftBottomLatLng.lng + mapScaleWidth, mapLeftBottomLatLng.lat + mapScaleHeight);
        // mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];
        //
		// mUserMaplayer = L.imageOverlay(mUserMap_url, mBounds, {attribution: mUserProject_Name, opacity: 0.5});
		// mMap.addLayer(mUserMaplayer, mUserProject_Name);
    };

    Indoor2DMap.prototype.update_node_pos = function(orig_this, NodeList)
    {
        for	(var index = 0; index < NodeList.length; index++)
        {
            var anchor_node = NodeList[index];
            if ( anchor_node == null)
                continue;
            var nodeLatLng = orig_this.convertToLatLng(orig_this, anchor_node.posX, anchor_node.posY);
            anchor_node.setLatLng(nodeLatLng);
        }
    };

    Indoor2DMap.prototype.setMapCoeff = function(map_origX, map_origY, width, height, scale, coeff, WGS48OriginX, WGS48OriginY, MAP_North)
    {
        if (map_origX == null )
            map_origX = 0.0;
        if (map_origY == null )
            map_origY = 0.0;
        this.mWGS48OriginX = parseFloat(WGS48OriginX);
        this.mWGS48OriginY = parseFloat(WGS48OriginY);
        this.mMAP_North = parseFloat(MAP_North);
        this.mUsermap_width = parseInt(width);
        this.mUsermap_height = parseInt(height);
        this.mMap_scale = parseFloat(scale);
        this.mMap_coeff = parseFloat(coeff);
        this.mMap_OrigX = parseFloat(map_origX);
        this.mMap_OrigY = parseFloat(map_origY);

        //console.log("=======================================================================================");

        //console.log("map_origX:"+map_origX+", map_origY:"+map_origY+", this.mMap_OrigX:"+this.mMap_OrigX+", mMap_OrigY:"+this.mMap_OrigY);
        //console.log("mWGS48OriginX:"+this.mWGS48OriginX+", mWGS48OriginY:"+this.mWGS48OriginY+", mMAP_North:"+this.mMAP_North);

        var mapScaleWidth = FloatDiv(this.mUsermap_width, this.mMap_scale * this.mMap_coeff);
        var mapScaleHeight = FloatDiv(this.mUsermap_height, this.mMap_scale * this.mMap_coeff);

        //console.log("mMap_scale:"+this.mMap_scale+", mMap_coeff:"+this.mMap_coeff+", mUsermap_width:"+this.mUsermap_width+", mUsermap_height:"+this.mUsermap_height);
        //console.log("mapScaleWidth:"+mapScaleWidth+", mapScaleHeight:"+mapScaleHeight);

        //====================================================================
        //var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
        //====================================================================
        var NodeLatitude = this.mWGS48OriginY + centimeterToLatitude(FloatMul(this.mMap_OrigY, -1.0));
        var NodeLongitude = this.mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(this.mMap_OrigX, -1.0));
        var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);

        var TenMeterLatitude = centimeterToLatitude(10000);
        var TenMeterLongitude = centimeterToLongitude(this.mWGS48OriginY + TenMeterLatitude, 10000);
        var ratio = TenMeterLatitude / TenMeterLongitude;
        //console.log("setMapCoeff() TenMeterLatitude:"+TenMeterLatitude +", TenMeterLongitude:"+TenMeterLongitude +", ratio:"+ratio);
        var mapScaleHeight = mapScaleHeight * ratio;
        //====================================================================

        var mapRightTopLatLng = xy(mapLeftBottomLatLng.lng + mapScaleWidth, mapLeftBottomLatLng.lat + mapScaleHeight);

        //console.log("mapLeftBottomLatLng:"+mapLeftBottomLatLng+", mapRightTopLatLng:"+mapRightTopLatLng);
        this.mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];

        // console.log("mBounds:"+this.mBounds);
        //console.log("=======================================================================================end");

    };

    Indoor2DMap.prototype.setMapImage = function(usr_map_url, project_name)
    {
        this.mUserMap_url = usr_map_url;
        this.mUserProject_Name = project_name;
        //=================
        //mUserMaplayer = L.imageOverlay(usr_map_url, mBounds, {attribution: project_name, opacity: 0.5});
        //=================
        this.update_2d_map(this);

        //mBounds = new L.LatLngBounds(rotatePointTopLeft, rotatePointTopRight).extend(rotatePointBottomLeft);
    	//mMap.fitBounds(mBounds);
        this.mMap.fitBounds(this.mBounds, {paddingTopLeft: [0,0]});

        //console.log("setMapImage() fitBounds mBounds:"+this.mBounds);

        //mUserMaplayer.reposition(rotatePointTopLeft, rotatePointTopRight, rotatePointBottomLeft);

        //
    	// control
    	//
    	this.mMap.zoomControl.setPosition('topright');
    	L.control.scale({imperial: false, maxWidth: 300}).addTo(this.mMap);

        // area layer:
        this.mLayerControl.addOverlay(this.layer_areas, "Areas");

        // show  Areas layer by default
        if (this.SHOW_AREA_DEFAULT == true)
            this.mMap.addLayer(this.layer_areas, "Areas");

        this.create_map_action(this.mMap);
    };

    Indoor2DMap.prototype.setMapLayerInfo = function(layerid)
    {
        this.mMapLayerId = layerid;
    };

    // Indoor2DMap.prototype.setBuildingInfo = function(layerid, building_id, building_floor)
    // {
    //     this.mMapLayerId = layerid;
    //     this.mBuildingID = building_id;
    //     this.mBuildingFloor = building_floor;
    // };

	//
	// change map
	//
	Indoor2DMap.prototype.changeUserMap = function(new_map_url, new_map_width, new_map_height)
	{
        this.mUserMap_url = new_map_url;
    	this.mMap.removeLayer(this.mUserMaplayer);

        this.mUsermap_width = new_map_width;
        this.mUsermap_height = new_map_height;

        var mapScaleWidth = FloatDiv(this.mUsermap_width, this.mMap_scale * this.mMap_coeff);
        var mapScaleHeight = FloatDiv(this.mUsermap_height, this.mMap_scale * this.mMap_coeff);
        //========================================================
        //var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
        //========================================================
        var NodeLatitude = this.mWGS48OriginY + centimeterToLatitude(FloatMul(this.mMap_OrigY, -1.0));
        var NodeLongitude = this.mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(this.mMap_OrigX, -1.0));
        var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);

        var TenMeterLatitude = centimeterToLatitude(10000);
        var TenMeterLongitude = centimeterToLongitude(this.mWGS48OriginY + TenMeterLatitude, 10000);
        var ratio = TenMeterLatitude / TenMeterLongitude;
        //console.log("update_2d_map() TenMeterLatitude:"+TenMeterLatitude +", TenMeterLongitude:"+TenMeterLongitude +", ratio:"+ratio);
        var mapScaleHeight = mapScaleHeight * ratio;
        //========================================================

        var mapRightTopLatLng = xy(mapLeftBottomLatLng.lng + mapScaleWidth, mapLeftBottomLatLng.lat + mapScaleHeight);

        this.mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];

        //console.log("changeUserMap() mBounds:"+mBounds);

    	this.mUserMaplayer = L.imageOverlay(this.mUserMap_url, this.mBounds, {attribution: this.mUserProject_Name, opacity: 0.5});
    	this.mMap.addLayer(this.mUserMaplayer, this.mUserProject_Name);
	};

    //
    // check current map editor mode
    //
    Indoor2DMap.prototype.editmode = function()
    {
        return this.mEdit_mode;
    };

    //
    // check current map editor mode
    //
    Indoor2DMap.prototype.setShowTrack = function(showrack)
    {
        this.mShowTrack = showrack;
    };

    Indoor2DMap.prototype.refresh = function()
    {
        var curZoom = this.mMap.getZoom();
        //console.log("refresh() curZoom:"+curZoom);

        this.mMap._onResize();

        if (curZoom == 0.0)
        {
            setTimeout(this.fitBounds, 100, this);
        }
    };


    Indoor2DMap.prototype.fitBounds = function(orig_this)
    {
        //console.log("Indoor2DMap.prototype.test_click() ");
        orig_this.mMap.fitBounds(orig_this.mBounds, {paddingTopLeft: [0,0]});
    };

    Indoor2DMap.prototype.setBounds = function(northeast_lat, northeast_lng, southwest_lat, southwest_lng)
    {
        var mapLeftBottomLatLng = xy(southwest_lng, southwest_lat);
        var mapRightTopLatLng = xy(northeast_lng, northeast_lat);
        this.mBounds = [mapLeftBottomLatLng, mapRightTopLatLng];
        this.mMap.fitBounds(this.mBounds, {paddingTopLeft: [0,0]});
    };

    Indoor2DMap.prototype.clearAddressTarget = function()
    {
        var current_layer = this.getCurrentLayer("Origin", this.mMapLayerId);
        for	(var index = 0; index < this.AddressList.length; index++)
        {
            var exist_address = this.AddressList[index];
            current_layer.removeLayer(exist_address);
        }

        this.AddressList = [];
    };

    Indoor2DMap.prototype.setTargetLocation = function(name, lat, lng)
    {
        var targetAddress;
        var found = false;
        for	(var index = 0; index < this.AddressList.length; index++)
        {
            var exist_address = this.AddressList[index];
            if (exist_address.name === name)
            {
                found = true;
                targetAddress = exist_address;
                break;
            }
        }

        if (found == false)
        {
            // ==================================
            var popName = name + "&nbsp;&nbsp;&nbsp;&nbsp;<br>";
            var markpopup = L.popup({
              closeOnClick: false,
              autoClose: false
            }).setContent(popName);

            var markerLatLng = xy(lng, lat);
            targetAddress = L.marker(markerLatLng, {title:name, draggable: false}).bindPopup(markpopup);
            targetAddress.name = name;
            targetAddress.orig_this = this;
            targetAddress.setIcon(origIcon);

            this.AddressList.push(targetAddress);

            targetAddress.on('click', this.markerOnClick);

            // dragend, drag
    		targetAddress.on('dragend', function(event){
    	    	var marker = event.target;
                var orig_this = marker.orig_this;
    	    	var position = marker.getLatLng();

                if (typeof orig_this.mTargetDragCallback === "function")
                {
                    // console.log("Marker.dragend()  type=" +marker.type + ", nodeid=" + marker.nodeid+ ", posX=" + marker.posX+ ", posY=" + marker.posY+ ", posZ=" + marker.posZ+ ", lat=" + position.lat+ ", lng=" + position.lng);
                    orig_this.mTargetDragCallback(orig_this.mMapLayerId, marker.name, position.lat, position.lng);
                }
    	  	});
            // ==================================

            var current_layer = this.getCurrentLayer("Origin", this.mMapLayerId);
            targetAddress.addTo(current_layer);
        }

        var targetLatLng = xy(lng, lat);
        targetAddress.setLatLng(targetLatLng);
    };

    //
    // check UI Smoothing
    //
    Indoor2DMap.prototype.setUISmoothing = function(UISmoothing)
    {
        if (UISmoothing && !this.mEnableUISmoothing)
        {
            setTimeout(this.draw_tag_smooth_move, 100, this);
            setTimeout(this.draw_tag_GPS_smooth_move, 100, this);
        }
        this.mEnableUISmoothing = UISmoothing;
    };

	Indoor2DMap.prototype.markerOnClick = function(e)
	{
        var marker = this;
	    marker.orig_this.markclick(e.target.type, e.target.index);

        if (typeof marker.orig_this.mNodeClickCallback === 'function')
            marker.orig_this.mNodeClickCallback(e.target);
	};

    Indoor2DMap.prototype.mapAreaOnClick = function(e)
	{
        var cur_layer = e;
        var orig_this = cur_layer.target.orig_this;
        //console.log("mapAreaOnClick() cur_layer:"+cur_layer.target.constructor.name);

	    if(orig_this.mEdit_mode && !orig_this.mEdit_delete_mode)
	    {
	       var layer = orig_this.MapAreas[cur_layer.target.index];
	       var shape = layer.toGeoJSON();
	       orig_this.toggle_map_area_editor(shape, cur_layer.target.index);
	    }
        else
        {
            var layer = orig_this.MapAreas[cur_layer.target.index];
            var shape = layer.toGeoJSON();
            if (shape.properties.areatype == 999) // 虛擬牆
            {
                // do nothing
            }
            else
            {
                fetch_rollcall(shape.properties.areaid, shape.properties.areaname);
            }
        }
	};

	//
	// create VILS marker
	// 123
	Indoor2DMap.prototype.createVILSMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, date, draggable, maplayer)
	{
        var popName = nodename + "&nbsp;&nbsp;&nbsp;&nbsp;<br>";
        if ( nodename.length == 0 )
            popName = nodeid + "&nbsp;&nbsp;&nbsp;&nbsp;<br>";

        //var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:false, maxWidth: "50"})
        //    .setContent("<img src=\"" + "/build/images/"+nodename+".png\"" + " width=\"40\" height=\"40\" />");

        var markpopup = L.popup({
          closeOnClick: false,
          autoClose: false
        }).setContent(popName);

        var markerLatLng = this.convertToLatLng(this, posX, posY);
        //var newMarker = L.marker(markerLatLng, {title:nodeid, draggable: draggable}).bindPopup(popName);
        var newMarker = L.marker(markerLatLng, {title:nodeid, draggable: draggable}).bindPopup(markpopup);
		newMarker.nodeid = nodeid;
        newMarker.macaddress = macaddress;
		newMarker.nodename = nodename;
		newMarker.posX = posX;
		newMarker.posY = posY;
		newMarker.posZ = posZ;
		newMarker.date = date;
		newMarker.orig_this = this;
        newMarker.maplayer = maplayer;

		newMarker.on('click', this.markerOnClick);

        // dragend, drag
		newMarker.on('dragend', function(event){
	    	var marker = event.target;
            var orig_this = marker.orig_this;
	    	var position = marker.getLatLng();

            //console.log("Marker.dragend() Enter...");
            //console.log("Marker.dragend() type=" +marker.type + ", nodeid=" + marker.nodeid+ ", position.lng=" + position.lng+ ", mWGS48OriginX=" + mWGS48OriginX);
            //console.log("Marker.dragend() type=" +marker.type + ", nodeid=" + marker.nodeid+ ", position.lat=" + position.lat+ ", mWGS48OriginY=" + mWGS48OriginY);


            //=======================
            // var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
            //
            // var latLngX = position.lng - mapLeftBottomLatLng.lng;
            // var latLngY = position.lat - mapLeftBottomLatLng.lat;
            //
            // marker.posX = latLngTocentimeter(latLngX);
            // marker.posY = latLngTocentimeter(latLngY);
            //=======================
	        // marker.posX = latLngTocentimeter(position.lng - mWGS48OriginX).toFixed(2);
	        // marker.posY = latLngTocentimeter(position.lat - mWGS48OriginY).toFixed(2);
            //=======================
            //marker.posX = latLngTocentimeter(position.lng).toFixed(2) - latLngX;
	        //marker.posY = latLngTocentimeter(position.lat).toFixed(2) - latLngY;
            //=======================
            // 2019.01.16
            marker.posY = LatitudeToCentimeter(position.lat) - LatitudeToCentimeter(orig_this.mWGS48OriginY);
            posX1 = LongitudeToCentimeter(position.lat, position.lng);
            posX2 = LongitudeToCentimeter(position.lat, orig_this.mWGS48OriginX);
            marker.posX = posX1 - posX2;
            //=======================
            //var onemeter = centimeterTolatLng(100);

            //console.log("Marker.dragend() marker.posX:" +marker.posX + " marker.posY:" + marker.posY);

            if (typeof orig_this.mNodeDragCallback === "function")
            {
                // console.log("Marker.dragend()  type=" +marker.type + ", nodeid=" + marker.nodeid+ ", posX=" + marker.posX+ ", posY=" + marker.posY+ ", posZ=" + marker.posZ+ ", lat=" + position.lat+ ", lng=" + position.lng);
                orig_this.mNodeDragCallback(orig_this.mMapLayerId, marker.type, newMarker.macaddress, marker.nodeid, marker.posX, marker.posY, marker.posZ, position.lat, position.lng);
            }
            else
            {
                //alert('typeof mNodeDragCallback not "function"');
            }
	  	});


		return newMarker;
	};

    //
    // fade out circle
    //
    Indoor2DMap.prototype.fadeout_tag_footprint = function(tag_node_id, maplayer)
    {
        var pointList = [];
        var existFadeOutNodes = [];

        var TagFadeOutline = this.FadeOutPolylines[tag_node_id];
        var TagFadeOutNodes = this.FadeOutNodes[tag_node_id];

        var current_layer = this.getCurrentLayer("Tags", maplayer);

        if (TagFadeOutline != null)
        {
            current_layer.removeLayer(TagFadeOutline);
        }

        if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
            return;

        var circleColor = 'rgb(244, 131, 66)';
        for(var i=0;i<TagFadeOutNodes.length;i++)
        {
            var circle = TagFadeOutNodes[i];
            var old_fop = circle.options.fillOpacity;
            var old_op = circle.options.opacitys;
            var old_radius = circle.options.radius;
            circleColor = circle.options.color;

            if (old_radius <= 0.5 || mShowTrack == false)
            {
                current_layer.removeLayer(circle);
            }
            else
            {
                circle.setStyle({radius:old_radius - 0.5});
                var position = circle.getLatLng();
                pointList.push([position.lat, position.lng]);
                existFadeOutNodes[ existFadeOutNodes.length ] = circle;
            }
        }
        TagFadeOutNodes = undefined;
        this.FadeOutNodes[tag_node_id] = existFadeOutNodes;

        TagFadeOutline = new L.Polyline(pointList, {
        color: circleColor,
        weight: 4,
        opacity: 0.4,
        smoothFactor: 1
        });
        current_layer.addLayer(TagFadeOutline);
        this.FadeOutPolylines[tag_node_id] = TagFadeOutline;
    };

    Indoor2DMap.prototype.fadeout_loop = function(orig_this)
    {
        for	(var index = 0; index < orig_this.Tags.length; index++)
        {
            var tag_node = orig_this.Tags[index];
            orig_this.fadeout_tag_footprint(tag_node.nodeid, tag_node.maplayer);
        }

        setTimeout(orig_this.fadeout_loop, 1000, orig_this);
    };

    Indoor2DMap.prototype.showGoolgeMap = function()
    {
        this.mMap.addLayer(gmapStreets, "Google Streets");
    };

    Indoor2DMap.prototype.showOriginLayer = function()
    {
        var current_layer = this.getCurrentLayer("Origin", this.mMapLayerId);
        this.mMap.addLayer(current_layer, "Origin");
    };

    Indoor2DMap.prototype.createMapEditor = function(callback)
    {
        this.mUsermapScaleDown = new this.UserMapScaleDownControl();
        this.mUsermapScaleUp = new this.UserMapScaleUpControl();
        this.mUsermapEdit = new this.UserMapEditControl();
        this.mMap.addControl(this.mUsermapEdit);
        this.fadeout_loop(this);

        this.mEditModeCallback = callback;
    };

    Indoor2DMap.prototype.setNodeDragCallback = function(callback)
    {
        this.mNodeDragCallback = callback;
    };

    Indoor2DMap.prototype.setOrigNodeDragCallback = function(callback)
    {
        this.mOrigNodeDragCallback  = callback;
    };

    Indoor2DMap.prototype.setUpdateMapInfoCallback = function(callback)
    {
        this.mUpdateMapInfoCallback = callback;
    };

    Indoor2DMap.prototype.setNodeClickCallback = function(callback)
    {
        this.mNodeClickCallback = callback;
    };

    //
	// add origin marker
	//
    Indoor2DMap.prototype.addOriginMarker = function(posX, posY, posZ)
    {
        var newMarker = this.createVILSMarker('0000', 'OriginXYZID', 'Origin', posX, posY, posZ, null, false, this.mMapLayerId);

		newMarker.setIcon(origIcon);
        var current_layer = this.getCurrentLayer("Origin", this.mMapLayerId);
		newMarker.addTo(current_layer);
		this.OriginMarker = newMarker;
		newMarker.type = 'Origin';
		newMarker.index = 0;

        // dragend, drag
        newMarker.on('dragend', function(event){
        	var marker = event.target;
        	var position = marker.getLatLng();

            var orig_this = this.orig_this;

            //console.log("OriginMarker:dragend() position:"+position + " orig_this.mMapLayerId:"+orig_this.mMapLayerId);

            //=======================
            //var mapLeftBottomLatLng = convertToLatLng(FloatMul(mMap_OrigX, -1.0), FloatMul(mMap_OrigY, -1.0));
            //=======================
            // var NodeLatitude = mWGS48OriginY + centimeterToLatitude(FloatMul(mMap_OrigY, -1.0));
            // var NodeLongitude = mWGS48OriginX + centimeterToLongitude(NodeLatitude, FloatMul(mMap_OrigX, -1.0));
            // var mapLeftBottomLatLng = xy(NodeLongitude, NodeLatitude);
            //
            // var latLngX = position.lng - mapLeftBottomLatLng.lng;
            // var latLngY = position.lat - mapLeftBottomLatLng.lat;
            //=======================
            // var latLngX = position.lng - mWGS48OriginX;
            // var latLngY = position.lat - mWGS48OriginY;
            //=======================

            //console.log("OriginMarker:dragend() latLngX:"+latLngX+", latLngY:"+latLngY);
            //console.log("OriginMarker:dragend() before mMap_OrigX:"+mMap_OrigX+", mMap_OrigY:"+mMap_OrigY);

            //========================
            // mMap_OrigX = latLngTocentimeter(latLngX);
            // mMap_OrigY = latLngTocentimeter(latLngY);
            //========================
            // mMap_OrigY = LatitudeToCentimeter(latLngY);
            // mMap_OrigX = LongitudeToCentimeter(latLngY, latLngX);
            //=======================
            // 2019.01.16
            var shiftY = LatitudeToCentimeter(position.lat) - LatitudeToCentimeter(orig_this.mWGS48OriginY);
            posX1 = LongitudeToCentimeter(position.lat, position.lng);
            posX2 = LongitudeToCentimeter(position.lat, orig_this.mWGS48OriginX);
            var shiftX = posX1 - posX2;

            orig_this.mMap_OrigY = orig_this.mMap_OrigY + shiftY;
            orig_this.mMap_OrigX = orig_this.mMap_OrigX + shiftX;
            //========================

            //console.log("OriginMarker:dragend() after mMap_OrigX:"+mMap_OrigX+", mMap_OrigY:"+mMap_OrigY);
            //console.log("OriginMarker:dragend() after  shiftX:"+shiftX+", shiftY:"+shiftY +", mMap_OrigX:"+mMap_OrigX+", mMap_OrigY:"+mMap_OrigY);

            //
            // update imageOverlay
            //
            orig_this.update_2d_map(orig_this);

            // reset OriginMarker to Origin
            marker.setLatLng(new L.LatLng(orig_this.mWGS48OriginY, orig_this.mWGS48OriginX));

            orig_this.mMap.panTo(marker.getLatLng());

            if (typeof orig_this.mOrigNodeDragCallback === 'function')
            {
                orig_this.mOrigNodeDragCallback(orig_this.mMapLayerId, orig_this.mMap_OrigX, orig_this.mMap_OrigY);
            }
        });
	};

    //
	// add origin marker
	//
    Indoor2DMap.prototype.createDummyTag = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
	{
        var found = this.updateMarker(this.Tags, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false);
        if (!found)
        {
            this.addTag(this.Tags.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date);
        }
    };

	//
	// add new Anchor marker
	//
	Indoor2DMap.prototype.addAnchor = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
	{
		var newMarker = this.createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false, maplayer);

        //
        // find same build and floor
        //
        var current_layer = this.getCurrentLayer("Anchors", maplayer);

		newMarker.setIcon(anchorIcon);
		newMarker.addTo(current_layer);
		this.Anchors[index] = newMarker;
		newMarker.type = 'anchor';
		newMarker.index = index;
	};

	//
	// add new Tag marker
	//
	Indoor2DMap.prototype.addTag = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
	{
		var newMarker = this.createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false, maplayer);
        //
        // find same build and floor
        //
        var current_layer = this.getCurrentLayer("Tags", maplayer);

		newMarker.setIcon(tagIcon);
        //newMarker.addTo(current_layer).openPopup();
        current_layer.addLayer(newMarker);
        //newMarker.openPopup();
        this.Tags[index] = newMarker;
		newMarker.type = 'tag';
		newMarker.index = index;

        //console.log("addTag() nodeid:"+nodeid +", index:"+index);
	};

	//
	// add new Coordinator marker
	//
	Indoor2DMap.prototype.addCoordinator = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
	{
		var newMarker = this.createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false, maplayer);

        //
        // find same build and floor
        //
        var current_layer = this.getCurrentLayer("Coordinators", maplayer);

		newMarker.setIcon(coordIcon);
		newMarker.addTo(current_layer);
	    this.Coords[index] = newMarker;
		newMarker.type = 'coord';
		newMarker.index = index;

	};

	//
	// add new locator marker
	//
	Indoor2DMap.prototype.addLocator = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
	{
		var newMarker = this.createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false, maplayer, maplayer);

        //
        // find same build and floor
        //
        var current_layer = this.getCurrentLayer("Locators", maplayer);

		newMarker.setIcon(locatorIcon);
        newMarker.addTo(current_layer);
	    this.Locators[index] = newMarker;
		newMarker.type = 'locator';
		newMarker.index = index;
	};

    Indoor2DMap.prototype.getCurrentLayer = function(prefix, maplayer)
    {
        //var target_layer = prefix +"_" + maplayer;
        var target_layer = prefix;
        var current_layer = this.mMapLayer_nodes[target_layer];
        if (current_layer == undefined)
        {
            //alert("layer_building_anchors["+target_layer+"] == undefined");
            current_layer = new L.LayerGroup();
            this.mMapLayer_nodes[target_layer] = current_layer;
            if ( prefix == "Tags")
            {
                // show tags layer by default
                this.mMap.addLayer(current_layer, target_layer);
            }
            if (prefix == "Locators" && this.SHOW_LOCATOR_DEFAULT == true)
            {
                // show Locators layer by default
                this.mMap.addLayer(current_layer, target_layer);
            }
            if (prefix == "Sensors")
            {
                // show Locators layer by default
                this.mMap.addLayer(current_layer, target_layer);
            }
            if (prefix == "Users")
            {
                // show Locators layer by default
                this.mMap.addLayer(current_layer, target_layer);
            }
            this.mLayerControl.addOverlay(current_layer, target_layer);
        }
        return current_layer;
    };

    // Indoor2DMap.prototype.getCurrentLayer = function(prefix, buildingid, floor)
    // {
    //     var target_layer = prefix +"_" + buildingid + "_" + floor;
    //     var current_layer = this.mMapLayer_nodes[target_layer];
    //     if (current_layer == undefined)
    //     {
    //         //alert("layer_building_anchors["+target_layer+"] == undefined");
    //         current_layer = new L.LayerGroup();
    //         this.mMapLayer_nodes[target_layer] = current_layer;
    //         if ( prefix ==  "Tags")
    //         {
    //             // show tags layer by default
    //             this.mMap.addLayer(current_layer, target_layer);
    //         }
    //         if (prefix ==  "Locators" && this.SHOW_LOCATOR_DEFAULT == true)
    //         {
    //             // show Locators layer by default
    //             this.mMap.addLayer(current_layer, target_layer);
    //         }
    //
    //         this.mLayerControl.addOverlay(current_layer, target_layer);
    //     }
    //     return current_layer;
    // };

  	Indoor2DMap.prototype.updateAnchorMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
    {
        //
        // check building id and floor
        //
        // if ( (this.mBuildingID != 99 && buildingid != 99) && (buildingid != this.mBuildingID || floor != this.mBuildingFloor))
        // {
        //     // not in this map
        //     return true;
        // }

        //console.log("updateAnchorMarker() nodeid:"+nodeid+" maplayer:"+maplayer +" this.mMapLayerId:"+this.mMapLayerId);

        //
        // check map layer id
        //
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            //this.setMarkerVisible(this.Anchors, nodeid, false);
            //this.updateMarker(this.Anchors, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, false, maplayer);
            return true;
        }

        var found = this.updateMarker(this.Anchors, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false, true, maplayer);
        if (!found)
        {
            this.addAnchor(this.Anchors.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
        }
    };

  	Indoor2DMap.prototype.updateTagMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, showGPSPos, maplayer)
    {
        //
        // check building id and floor
        //
        // console.log("updateTagMarker() " + nodeid+" ("+posX+","+posY+","+posZ+") floor:"+floor +" buildingid:"+buildingid);
        //console.log("updateTagMarker() nodeid:" + nodeid+" maplayer:"+maplayer+" this.mMapLayerId:"+this.mMapLayerId);

        //if ( (this.mBuildingID != 99 && buildingid != 99) && (buildingid != this.mBuildingID || floor != this.mBuildingFloor))
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            //alert("hideMarker:"+macaddress+":"+nodeid);
            this.setMarkerVisible(this.Tags, nodeid, false);
            //var changeState = setMarkerVisible(Tags, nodeid, false);
            // if ( changeState )
            // {
            //     console.log("updateTagMarker() " + nodeid + " visible to hide!!!");
            // }
            this.updateMarker(this.Tags, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, false, maplayer);
            this.push_to_drawlist(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
            return true;
        }
        else
        {
            var visibleState = this.check_visible_state(this.Tags, nodeid);
            if ( visibleState == false && !showGPSPos)
            {
                //console.log("updateTagMarker() " + nodeid + " hide to visible!!!");
                this.reset_node(nodeid, posX, posY, posZ, buildingid, floor, maplayer);
                this.updateMarker(this.Tags, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, true, maplayer);
                this.setMarkerVisible(this.Tags, nodeid, true);
            }
        }

        //
        // for draw tag
        //
        if (showGPSPos)
        {
            this.hide_2d_drawlist(nodeid);
        }
        else
        {
            this.hide_2d_drawGPSlist(nodeid);
            this.push_to_drawlist(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
        }

        var updateUI = (!this.mEnableUISmoothing && !showGPSPos);

        // if ( nodeid == 'Tag0013' )
        // {
        //     console.log("updateTagMarker() Tag0013 updateUI:" + updateUI +", showGPSPos:"+showGPSPos);
        // }

        var found = this.updateMarker(this.Tags, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, updateUI, maplayer);
        if (!found)
        {
            this.addTag(this.Tags.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
        }
        return found;
    };

  	Indoor2DMap.prototype.updateCoordinatorMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
    {
        //
        // check building id and floor
        //
        // if ( (this.mBuildingID != 99 && buildingid != 99) && (buildingid != this.mBuildingID || floor != this.mBuildingFloor))
        // {
        //     // not in this map
        //     return true;
        // }

        //
        // check map layer id
        //
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            this.setMarkerVisible(this.Coords, nodeid, false);
            this.updateMarker(this.Coords, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, false, maplayer);
            return true;
        }

        var found = this.updateMarker(this.Coords, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false, true, maplayer);
        if (!found)
        {
            this.addCoordinator(this.Coords.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
        }
    };

  	Indoor2DMap.prototype.updateLocatorMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer)
    {
        //
        // check building id and floor
        //
        // if ( (this.mBuildingID != 99 && buildingid != 99) && (buildingid != this.mBuildingID || floor != this.mBuildingFloor))
        // {
        //     // not in this map
        //     return true;
        // }

        //
        // check map layer id
        //
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            this.setMarkerVisible(this.Locators, nodeid, false);
            this.updateMarker(this.Locators, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true, false, maplayer);
            return true;
        }

        var found = this.updateMarker(this.Locators, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false, true, maplayer);
        if (!found)
        {
            this.addLocator(this.Locators.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
        }
        return found;
    };

  	Indoor2DMap.prototype.updateMarker = function(nodeList, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, fadeoutFootprint, updateUI, maplayer)
    {
        var found = false;
        var exist_node = null;

        // find the exist node
        var index;
        for	(index = 0; index < nodeList.length; index++)
        {
            var exist_node = nodeList[index];
            if ( exist_node.nodeid === nodeid )
            {
                var node_pre_buildingid = exist_node.buildingid;
                var node_pre_floor = exist_node.floor;

                //replace with new data
                if ( nodename.length == 0 )
                    exist_node.nodename = nodeid;
                else
                    exist_node.nodename = nodename;
                exist_node.posX = posX;
                exist_node.posY = posY;
                exist_node.posZ = posZ;
                exist_node.buildingid = buildingid;
                exist_node.floor = floor;
                exist_node.maplayer = maplayer;

                //convert to local time
                exist_node.date = date;
                exist_node.macaddress = macaddress;


                if (updateUI)
                {
                    //if (itemindex == 0)
                    //    $("#testMarkerUpdate").text("("+node.posX+","+node.posY+","+node.posZ+"), data.length="+data.length);
                    //
                    // update UI
                    //
                    var nodeLatLng = this.convertToLatLng(this, posX, posY);
                    exist_node.setLatLng(nodeLatLng);

                    //
                    // create fadeout foot print
                    //
                    if (fadeoutFootprint && this.mShowTrack)
                    {
                        //console.log("updateMarker() nodeid:"+ exist_node.nodeid +" _icon:" + exist_node._icon);

                        if (exist_node._icon != undefined && exist_node._icon.style.visibility == 'hidden')
                        {
                            this.showMarker(exist_node);
                        }

                        var colorIdx = index;
                        if (colorIdx >= 10)
                        {
                            colorIdx -= 10;
                        }
                        var curColor = footprintcolors[colorIdx];
                        var fillColor = footprintcolors[colorIdx];

                        var circle = L.circleMarker(nodeLatLng, { color:curColor, opacity: 0.3, fillColor:fillColor, fillOpacity: 0.2, className: "geoData", radius: 5 });
                        //circle.type = 'circleMarker';
                        var current_layer = this.getCurrentLayer("Tags", maplayer);
                        circle.buildingid = buildingid;
                        circle.floor = floor;
                        circle.addTo(current_layer);

                        var TagFadeOutNodes = this.FadeOutNodes[exist_node.nodeid];
                        if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
                            TagFadeOutNodes = [];
                        TagFadeOutNodes[ TagFadeOutNodes.length ] = circle;
                        this.FadeOutNodes[exist_node.nodeid] = TagFadeOutNodes;
                    }
                }
                found = true;
                break;
            }
        }
        return found;
    };

  	Indoor2DMap.prototype.updateSensorMarker = function(id, name, type, Longitude, Latitude, maplayer, Battery, unknownLocation)
    {
        // console.log("updateSensorMarker() id:"+ id +" name:" + name +" Longitude:" + Longitude +" Latitude:" + Latitude +" maplayer:" + maplayer +" Battery:" + Battery +" unknownLocation:" + unknownLocation);
        var current_layer = this.getCurrentLayer("Sensors", maplayer);

        //
        // check map layer id
        //
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            return true;
        }

        var bFound = false;
        for	(var index = 0; index < this.Sensors.length; index++)
        {
            var exist_sensor = this.Sensors[index];
            if ( exist_sensor.id === id )
            {
                bFound = true;
                exist_sensor.setLatLng(xy(Longitude, Latitude));
                break;
            }
        }

        if (bFound == false)
        {
            var date = new moment();
            var newMarker = this.createVILSMarker(id, id, name, 0, 0, 0, date, false, maplayer);
            if ( type === 'LiDAR' )
                newMarker.setIcon(LiDARIcon);
            else
            {
                // console.log("updateSensorMarker() bFound == false name:" + name +" Battery:"+ type +" Battery:" + Battery +" unknownLocation:" + unknownLocation);

                if (Battery.length == 0 || Battery == 0)
                    newMarker.setIcon(smartWatchLowBatteryIcon);
                else
                if (unknownLocation == 1)
                    newMarker.setIcon(smartWatchUnknownLocationIcon);
                else
                    newMarker.setIcon(smartWatchIcon);
            }
            newMarker.setLatLng(xy(Longitude, Latitude));

            current_layer.addLayer(newMarker);

            var new_index = this.Sensors.length;
            this.Sensors[new_index] = newMarker;
            newMarker.id = id;
            newMarker.type = 'sensor';
            newMarker.index = new_index;
        }
        else
        {
            if ( type === 'LiDAR' )
                exist_sensor.setIcon(LiDARIcon);
            else
            {
                // console.log("updateSensorMarker() bFound == true name:" + name +" Battery:"+ type +" Battery:" + Battery +" unknownLocation:" + unknownLocation);

                if (Battery.length == 0 || Battery == 0)
                    exist_sensor.setIcon(smartWatchLowBatteryIcon);
                else
                if (unknownLocation == 1)
                    exist_sensor.setIcon(smartWatchUnknownLocationIcon);
                else
                    exist_sensor.setIcon(smartWatchIcon);
            }
        }
    };

  	Indoor2DMap.prototype.updateUserMarker = function(userid, name, maplayer, posX, posY, posZ, Longitude, Latitude)
    {
        // console.log("updateSensorMarker() id:"+ id +" name:" + name +" Longitude:" + Longitude +" Latitude:" + Latitude +" maplayer:" + maplayer +" Battery:" + Battery +" unknownLocation:" + unknownLocation);
        var current_layer = this.getCurrentLayer("Users", maplayer);

        // console.log("updateSensorMarker() this.mMapLayerId:"+this.mMapLayerId+" maplayer:"+maplayer);

        //
        // check map layer id
        //
        if ( (this.mMapLayerId != 0 && maplayer != 0) && (this.mMapLayerId != maplayer))
        {
            // not in this map
            return true;
        }

        var nodeLatLng = xy(Longitude, Latitude);
        var int_Longitude = parseInt(Longitude);
        var int_Latitude = parseInt(Latitude);
        if( int_Longitude == 0 || int_Latitude == 0)
        {
            nodeLatLng = this.convertToLatLng(this, posX, posY);
        }

        var bFound = false;
        for	(var index = 0; index < this.Users.length; index++)
        {
            var exist_user = this.Users[index];
            if ( exist_user.id === userid )
            {
                bFound = true;
                exist_user.setLatLng(nodeLatLng);
                break;
            }
        }

        if (bFound == false)
        {
            var date = new moment();
            var newMarker = this.createVILSMarker(userid, userid, name, 0, 0, 0, date, false, maplayer);
            newMarker.setIcon(UserIcon);
            newMarker.setLatLng(nodeLatLng);

            current_layer.addLayer(newMarker);

            var new_index = this.Users.length;
            this.Users[new_index] = newMarker;
            newMarker.id = userid;
            newMarker.type = 'user';
            newMarker.index = new_index;
        }
    };

    Indoor2DMap.prototype.getTagList = function()
    {
        return this.Tags;
    };

    Indoor2DMap.prototype.getLocatorList = function()
    {
        return this.Locators;
    };

    Indoor2DMap.prototype.setTagPopup = function(nodeid, showState)
    {
        var index;
		for	(index = 0; index < this.Tags.length; index++)
		{
			var exist_node = this.Tags[index];

            //console.log("setTagPopup() index:"+index+" nodeid:"+nodeid +" exist_node.nodeid:"+exist_node.nodeid);

			if ( exist_node.nodeid === nodeid )
			{
                if (showState)
                {
                    exist_node.openPopup();
                    this.mMap.setView(exist_node.getLatLng());
                    //this.mMap.setView(exist_node.getLatLng(), this.popupZoom);
                }
                else
                   exist_node.closePopup();

                break;
            }
        }
    };

    Indoor2DMap.prototype.setLocatorPopup = function(nodeid, showState)
    {
        var index;
		for	(index = 0; index < this.Locators.length; index++)
		{
			var exist_node = this.Locators[index];

            //console.log("setTagPopup() index:"+index+" nodeid:"+nodeid +" exist_node.nodeid:"+exist_node.nodeid);

			if ( exist_node.nodeid === nodeid )
			{
                if (showState)
                {
                    exist_node.openPopup();
                    this.mMap.setView(exist_node.getLatLng());
                    //this.mMap.setView(exist_node.getLatLng(), this.popupZoom);
                }
                else
                   exist_node.closePopup();

                break;
            }
        }
    };

    Indoor2DMap.prototype.setSensorPopup = function(id, showState)
    {
        var index;
		for	(index = 0; index < this.Sensors.length; index++)
		{
			var exist_sensor = this.Sensors[index];

            console.log("setSensorPopup() index:"+index+" id:"+id +" exist_sensor.id:"+exist_sensor.id);

			if ( exist_sensor.id === id )
			{
                if (showState)
                {
                    exist_sensor.openPopup();
                    this.mMap.setView(exist_sensor.getLatLng());
                    //this.mMap.setView(exist_sensor.getLatLng(), this.popupZoom);
                }
                else
                   exist_sensor.closePopup();

                break;
            }
        }
    };

    Indoor2DMap.prototype.setUserPopup = function(id, showState)
    {
        var index;
		for	(index = 0; index < this.Users.length; index++)
		{
			var exist_user = this.Users[index];

			if ( exist_user.id == id )
			{
                // console.log("setUserPopup() index:"+index+" id:"+id +" exist_user.id:"+exist_user.id +" showState:"+showState);

                if (showState)
                {
                    exist_user.setIcon(UserIcon);
                    exist_user.openPopup();
                    this.mMap.setView(exist_user.getLatLng());
                    //this.mMap.setView(exist_user.getLatLng(), this.popupZoom);
                }
                else
                {
                    exist_user.setIcon(LiDARIcon);
                    exist_user.closePopup();
                }

                break;
            }
        }
    };

	Indoor2DMap.prototype.setTagAlert = function(nodeid, alertType)
	{
		// find the exist node
        this.set_TagDraw_Alert(nodeid, alertType);
        this.set_TagGPSDraw_Alert(nodeid, alertType);
	};

    Indoor2DMap.prototype.setStartAlert = function(nodeid)
	{
        if ( this.NodeAlertFunction[nodeid] == undefined)
        {
            this.NodeAlertFunction[nodeid] = {};
        }
        this.NodeAlertFunction[nodeid] = 1; // 0:disable , 1:enable

        console.log("setStartAlert() " + nodeid);
	};

    Indoor2DMap.prototype.setStopAlert = function(nodeid)
	{
        if ( this.NodeAlertFunction[nodeid] == undefined)
        {
            this.NodeAlertFunction[nodeid] = {};
        }
        this.NodeAlertFunction[nodeid] = 0; // 0:disable , 1:enable

        var TargetNode = this.getTargetNode(nodeid);
        TargetNode.setIcon(this.AlertIconScaleUpDownList[0]);

        console.log("setStopAlert() " + nodeid);
	};

    Indoor2DMap.prototype.check_tag_layer_alertcircle = function()
    {
        var current_layer = this.getCurrentLayer("Tags", this.mMapLayerId);

        var layers = current_layer.getLayers();
        //console.log("check_tag_layer_alertcircle() layers.length:"+ layers.length);

        var index;
        for(index=0; index<layers.length;index++)
        {
            var layer = layers[index];
            //console.log("check_tag_layer_alertcircle() layer.type:"+ layer.type);

            if (layer.type === 'circleMarker')
            {
                console.log("check_tag_layer_alertcircle() Found!");
                current_layer.removeLayer(layer);
            }
        }
    };

    Indoor2DMap.prototype.check_locator_layer_alertcircle = function()
    {
        var current_layer = this.getCurrentLayer("Locators", this.mMapLayerId);

        var layers = current_layer.getLayers();
        //console.log("check_locator_layer_alertcircle() layers.length:"+ layers.length);

        for(var index=0; index<layers.length;index++)
        {
            var layer = layers[index];
            //console.log("check_tag_layer_alertcircle() layer.type:"+ layer.type);

            if (layer.type === 'circleMarker')
            {
                console.log("check_locator_layer_alertcircle() Found!");
                current_layer.removeLayer(layer);
            }
        }
    };

    Indoor2DMap.prototype.set_TagDraw_Alert = function(nodeid, alertType)
    {
        //console.log("set_TagDraw_Alert() " + nodeid);

		for	(var index=0; index < this.TagDrawList.length; index++)
		{
			var exist_node = this.TagDrawList[index];


			// if ( alertType > 0 )
			// {
			// 		console.log("setTagAlert exist_node.nodeid:" + exist_node.nodeid);
			// }

			//if ( exist_node.nodeid == nodeid )
            if ( exist_node.nodeid === nodeid )
			{
                var TargetNode = this.getTargetNode(nodeid);
                if (TargetNode == null)
                {
                	//console.log("set_TagDraw_Alert() nodeid:" + nodeid +" TargetNode == null");
                    return ;
                }
                // if ( alertType > 0 )
				// {
				// 	console.log("set_TagDraw_Alert exist_node.nodeid:" + exist_node.nodeid +" TargetNode.maplayer:"+TargetNode.maplayer +" this.mMapLayerId:"+this.mMapLayerId);
				// }
                //if ( (exist_node.buildingid != this.mBuildingID || exist_node.floor != this.mBuildingFloor))
                if ( (TargetNode.maplayer != this.mMapLayerId))
                {
                    // not in this map
                    //console.log("setTagAlert() " + nodeid +" not in this map");
                    return;
                }


				if (exist_node.alertcount > 0)
						exist_node.alertcount--;

                // if ("Tag340" === nodeid)
                // {
                //     console.log("set_TagDraw_Alert() " + nodeid +" exist_node.alertcount:"+exist_node.alertcount +", alertType:"+alertType);
                // }

                //
                // check show alert function
                //
                if ( this.NodeAlertFunction[nodeid] == undefined)
                {
                    this.NodeAlertFunction[nodeid] = {};
                    this.NodeAlertFunction[nodeid] = 1; //enable
                }

                //console.log("set_TagDraw_Alert() " + nodeid +" exist_node.alertcount:"+exist_node.alertcount +", alertType:"+alertType +", NodeAlertFunction[nodeid]:"+NodeAlertFunction[nodeid]);

				if ( alertType == 0 || this.NodeAlertFunction[nodeid] == 0)
				{
                    exist_node.alertcount = 0;

					var current_layer = this.getCurrentLayer("Tags", this.mMapLayerId);
					var circle = exist_node.alertcircle;
					if ( circle != undefined && circle != null)
					{
                        current_layer.removeLayer(circle);
                        exist_node.alertcircle = null;

                        TargetNode.closePopup();
					}

                    // var markpopup = L.popup({closeOnClick: false, autoClose: false}).setContent(nodeid);
                    // Tags[index].bindPopup(markpopup);
                    TargetNode.setPopupContent(TargetNode.nodename);

					break;
				}

				var nodeLatLng = this.convertToLatLng(this, exist_node.curposX, exist_node.curposY);

				var curColor = 'rgb(244, 20, 20)';
				var fillColor = 'rgb(244, 20, 20)';
				if (alertType == 2)
				{
						//0D alert
						curColor = 'rgb(244, 20, 20)';
						fillColor = 'rgb(244, 20, 20)';
						//curColor = 'rgb(184, 180, 100)';
						//fillColor = 'rgb(184, 180, 100)';
				}

				var circle = exist_node.alertcircle;
				//console.log("setTagAlert " + exist_node.nodeid + " circle:"+circle);

				if ( circle == undefined || circle == null)
				{
					circle = L.circleMarker(nodeLatLng, { color:curColor, opacity: 0.2, fillColor:fillColor, fillOpacity: 0.2, className: "geoData", radius: 25 });
                    circle.type = 'circleMarker';
                    var current_layer = this.getCurrentLayer("Tags", this.mMapLayerId);
					circle.addTo(current_layer);
					exist_node.alertcircle = circle;
					exist_node.alertcount = 50;
					//console.log("set_TagDraw_Alert()" + exist_node.nodeid + " circle:"+circle + " alertcount:"+exist_node.alertcount +" alertType:"+alertType);

                    // var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, autoClose: false, maxWidth: "190"})
                    //           .setContent(nodeid +" 按下警急按鈕");
                    // Tags[index].bindPopup(markerpopup);

                    TargetNode.setPopupContent(TargetNode.nodename +" 按下警急按鈕");
                    TargetNode.openPopup();
                    this.mMap.setView(TargetNode.getLatLng());
                    //this.mMap.setView(TargetNode.getLatLng(), this.popupZoom);
                }

				break;
			}
		}
    };

    Indoor2DMap.prototype.set_TagGPSDraw_Alert = function(nodeid, alertType)
    {
        //console.log("set_TagGPSDraw_Alert() " + nodeid);

		for	(var index = 0; index < this.TagGPSDrawList.length; index++)
		{
			var exist_node = this.TagGPSDrawList[index];
			if ( exist_node.nodeid === nodeid )
			{
                var TargetNode = this.getTargetNode(nodeid);
                if (TargetNode == null)
                {
                	console.log("set_TagGPSDraw_Alert() nodeid:" + nodeid +" TargetNode == null");
                    return ;
                }

                //if ( (exist_node.buildingid != this.mBuildingID || exist_node.floor != this.mBuildingFloor))
                if ( TargetNode.maplayer != this.mMapLayerId )
                {
                    // not in this map
                    console.log("set_TagGPSDraw_Alert() " + nodeid +" not in this map");
                    return;
                }

				if (exist_node.alertcount > 0)
					exist_node.alertcount--;

                //
                // check show alert function
                //
                if ( this.NodeAlertFunction[nodeid] == undefined)
                {
                    this.NodeAlertFunction[nodeid] = {};
                    this.NodeAlertFunction[nodeid] = 1; //enable
                }

                //console.log("set_TagGPSDraw_Alert() " + nodeid +" exist_node.alertcount:"+exist_node.alertcount +", alertType:"+alertType +", NodeAlertFunction[nodeid]:"+NodeAlertFunction[nodeid]);

				if ( alertType == 0 || this.NodeAlertFunction[nodeid] == 0)
				{
                    exist_node.alertcount = 0;
					var current_layer = this.getCurrentLayer("Tags", this.mMapLayerId);
					var circle = exist_node.alertcircle;
					if ( circle != undefined && circle != null)
					{
                        current_layer.removeLayer(circle);
						exist_node.alertcircle = null;

                        TargetNode.closePopup();
					}

                    // var markpopup = L.popup({closeOnClick: false, autoClose: false}).setContent(nodeid);
                    // Tags[index].bindPopup(markpopup);
                    TargetNode.setPopupContent(TargetNode.nodename);

					break;
				}

                //console.log("set_TagGPSDraw_Alert() " + nodeid +" curposLng:"+exist_node.curposLng+", curposLat:"+exist_node.curposLat);
                var nodeLatLng = xy(exist_node.curposLng, exist_node.curposLat);

				var curColor = 'rgb(244, 20, 20)';
				var fillColor = 'rgb(244, 20, 20)';
				if (alertType == 2)
				{
					//0D alert
					curColor = 'rgb(244, 20, 20)';
					fillColor = 'rgb(244, 20, 20)';
					//curColor = 'rgb(184, 180, 100)';
					//fillColor = 'rgb(184, 180, 100)';
				}

				var circle = exist_node.alertcircle;
				//console.log("set_TagGPSDraw_Alert() " + exist_node.nodeid + " circle:"+circle);

				if ( circle == undefined || circle == null)
				{
                    circle = L.circleMarker(nodeLatLng, { color:curColor, opacity: 0.2, fillColor:fillColor, fillOpacity: 0.2, className: "geoData", radius: 25 });
                    circle.type = 'circleMarker';
                    var current_layer = this.getCurrentLayer("Tags", this.mMapLayerId);
					circle.addTo(current_layer);
					exist_node.alertcircle = circle;
					exist_node.alertcount = 50;
					console.log("set_TagGPSDraw_Alert()" + exist_node.nodeid + " circle:"+circle + " alertcount:"+exist_node.alertcount +" alertType:"+alertType);

                    // var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, autoClose: false, maxWidth: "190"})
                    //           .setContent(nodeid +" 按下警急按鈕");
                    // Tags[index].bindPopup(markerpopup);

                    TargetNode.bindPopup(nodeid +" 按下警急按鈕");
                    TargetNode.openPopup();
                    this.mMap.setView(TargetNode.getLatLng());
                    //this.mMap.setView(TargetNode.getLatLng(), this.popupZoom);
                }

				break;
			}
		}
    };

    Indoor2DMap.prototype.getTargetNode = function(nodeid)
    {
        var exist_node = null;
        for	(var index = 0; index < this.Tags.length; index++)
		{
			exist_node = this.Tags[index];
			if ( exist_node.nodeid === nodeid )
			{
                return exist_node;
            }
        }
        return exist_node;
    };

    Indoor2DMap.prototype.getTargetLocatorNode = function(nodeid)
    {
        var exist_node = null;
        for	(var index = 0; index < this.Locators.length; index++)
		{
			exist_node = this.Locators[index];
			if ( exist_node.nodeid === nodeid )
			{
                return exist_node;
            }
        }
        return exist_node;
    };

	Indoor2DMap.prototype.setTagLocation = function(nodeid, alertType, areaid, areaname, areaViewtype, alive, exclude_rollcall_AreaId, area_unknown, showGPSPos, voltage_low, asleep)
	{
        //console.log("setTagLocation()" + nodeid + " areaid:"+areaid + " areaname:"+areaname + " areaViewtype:"+areaViewtype);

        for	(var index = 0; index < this.Tags.length; index++)
        {
            var exist_node = this.Tags[index];
            if ( exist_node.nodeid === nodeid )
            {
                if (alertType != 0)
                {
                    if ( exist_node.alertcount == 0 )
                    {
                        // normal
                        if (voltage_low == 1)
                            exist_node.setIcon(tagLowBatteryIcon);
                        else
                            exist_node.setIcon(tagIcon);
                    }
                }
                else
                {
                    if ( areaid.length > 0 )
                    {
                        // console.log("setTagLocation()" + nodeid + " areaid:"+areaid + " areaname:"+areaname + " areaViewtype:"+areaViewtype);

                        if(areaViewtype == 0)
                        {
                            // normal
                            if (voltage_low == 1)
                                exist_node.setIcon(tagLowBatteryIcon);
                            else
                                exist_node.setIcon(tagIcon);
                        }
                        else
                        if(areaViewtype == 1)
                        {
                            // console.log("setTagLocation()" + nodeid + " areaid:"+areaid + " areaname:"+areaname + " areaViewtype:"+areaViewtype);
                            // room
                            if (voltage_low == 1)
                                exist_node.setIcon(grayLowBatteryIcon);
                            else
                                exist_node.setIcon(grayIcon);
                        }
                        else
                        if(areaViewtype == 2)
                        {
                            // console.log("setTagLocation()" + nodeid + " areaid:"+areaid + " areaname:"+areaname + " areaViewtype:"+areaViewtype);
                            // danger
                            exist_node.setIcon(alertIcon);
                        }
                    }

                    if ( asleep == 1 )
                    {
                        if (voltage_low == 1)
                            exist_node.setIcon(tagSleepLowBatteryIcon);
                        else
                            exist_node.setIcon(tagSleepIcon);
                    }
                    else
                    if ( alive == 0 || area_unknown == 1)
                    {
                        if (voltage_low == 1)
                            exist_node.setIcon(inactiveLowBatteryIcon);
                        else
                            exist_node.setIcon(inactiveIcon);
                    }
                    else
                    if( (exclude_rollcall_AreaId.length) > 0 && !showGPSPos )
                    {
                        if (voltage_low == 1)
                            exist_node.setIcon(norollcallLowBatteryIcon);
                        else
                            exist_node.setIcon(norollcallIcon);
                    }
                    else
                    {
                        if (voltage_low == 1)
                            exist_node.setIcon(tagLowBatteryIcon);
                        else
                            exist_node.setIcon(tagIcon);
                    }
                }
                break;
            }
        }
	};

	Indoor2DMap.prototype.setTagLngLat = function(nodeid, Longitude, Latitude, buildingid, floor)
	{
        this.setMarkerVisible(this.Tags, nodeid, true);

        if ( Longitude <= 0.0 && Latitude <= 0.0 )
        {
            // console.log("setTagLngLat() Longitude:"+Longitude+", Latitude:"+Latitude);
            return;
        }

        for	(var index = 0; index < this.Tags.length; index++)
        {
            var exist_node = this.Tags[index];
            if ( exist_node.nodeid === nodeid )
            {
                exist_node.setIcon(tagIcon);

                if(this.mEnableUISmoothing)
                {
                    this.push_to_GPSdrawlist(nodeid, Longitude, Latitude, buildingid, floor);
                }
                else
                {
                    exist_node.setLatLng(xy(Longitude, Latitude));
                }
            }
        }
    };

    Indoor2DMap.prototype.updateTagGPS = function(nodeid, Longitude, Latitude)
    {
        for	(var index = 0; index < this.Tags.length; index++)
        {
            var exist_node = this.Tags[index];
            if ( exist_node.nodeid === nodeid )
            {
                exist_node.setLatLng(xy(Longitude, Latitude));
            }
        }
    };

	Indoor2DMap.prototype.setLocatorAlert = function(nodeid, areaViewtype, alive, poweroff)
	{
        //if (nodeid == "Locator1009")
        //    console.log("setLocatorAlert() nodeid:" + nodeid + " areaViewtype:"+areaViewtype);
        if (areaViewtype >= 0)
        {
            for	(var index = 0; index < this.Locators.length; index++)
            {
                var exist_node = this.Locators[index];

                if ( exist_node.nodeid === nodeid )
				{
                    // if ( (exist_node.buildingid != this.mBuildingID || exist_node.floor != this.mBuildingFloor))
                    if ( exist_node.maplayer != this.mMapLayerId )
                    {
                        // not in this map
                        return;
                    }

                    if (exist_node.alertcount > 0)
                    {
                        exist_node.alertcount--;
                    }

                    //
                    // check show alert function
                    //
                    if ( this.NodeAlertFunction[nodeid] == undefined)
                    {
                        this.NodeAlertFunction[nodeid] = {};
                        this.NodeAlertFunction[nodeid] = 1; //enable
                    }

                    if ( areaViewtype == 0 || this.NodeAlertFunction[nodeid] == 0)
                    {
                        exist_node.alertcount = 0;
                        // normal icon
                        if (poweroff == 1 )
                        {
                            exist_node.setIcon(locatorPowerOffIcon);
                        }
                        else
                        if (alive > 0)
                        {
                            exist_node.setIcon(locatorIcon);
                        }
                        else
                            exist_node.setIcon(locatornotaliveIcon);

                        var current_layer = this.getCurrentLayer("Locators", this.mMapLayerId);
                        var circle = exist_node.alertcircle;
                        if ( circle != undefined && circle != null)
                        {
                            current_layer.removeLayer(circle);
                            exist_node.alertcircle = null;
                            exist_node.closePopup();
                        }

                        exist_node.setPopupContent(exist_node.nodename);
                        break;
                    }

                    //console.log("setLocatorAlert() posX:"+exist_node.posX+" posY:"+exist_node.posY);

					//var nodeLatLng = convertToLatLng(exist_node.curposX, exist_node.curposY);
                    var nodeLatLng = this.convertToLatLng(this, exist_node.posX, exist_node.posY);

					var curColor = 'rgb(244, 20, 20)';
					var fillColor = 'rgb(244, 20, 20)';
					if (areaViewtype == 2)
					{
						curColor = 'rgb(244, 20, 20)';
						fillColor = 'rgb(244, 20, 20)';
					}

					var circle = exist_node.alertcircle;
					if ( circle == undefined || circle == null)
					{
						circle = L.circleMarker(nodeLatLng, { color:curColor, opacity: 0.2, fillColor:fillColor, fillOpacity: 0.2, className: "geoData", radius: 25 });
                        circle.type = 'circleMarker';
                        var current_layer = this.getCurrentLayer("Locators", this.mMapLayerId);
						circle.addTo(current_layer);
						exist_node.alertcircle = circle;
						exist_node.alertcount = 50;
						//console.log("setLocatorAlert()" + exist_node.nodeid + " circle:"+circle + " alertcount:"+exist_node.alertcount +" alertType:"+alertType);

                        // var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, autoClose: false, maxWidth: "190"})
                        //           .setContent(nodeid +" 按下警急按鈕");
                        // exist_node.bindPopup(markerpopup);
                        exist_node.setPopupContent(exist_node.nodename +" 按下警急按鈕");
                        exist_node.openPopup();
                        this.mMap.setView(exist_node.getLatLng());
                        //this.mMap.setView(exist_node.getLatLng(), this.popupZoom);
                    }
					break;
                }
            }
        }
    };

    //
    // hide marker
    //
  	Indoor2DMap.prototype.setMarkerVisible = function(nodeList, nodeid, visible)
    {
        var changeState = false;

        // find the exist node
        for	(var index = 0; index < nodeList.length; index++)
        {
            var exist_node = nodeList[index];
            if ( exist_node.nodeid === nodeid )
            {
                if (visible)
                {
                    changeState = this.showMarker(exist_node);
                }
                else
                {
                    changeState = this.hideMarker(exist_node);
                }
                break;
            }
        }
        return changeState;
    };

    Indoor2DMap.prototype.hideMarker = function(node)
    {
        //console.log("hideMarker() nodeid:"+ node.nodeid +" _icon:" + node._icon);

        var changeState = false;

        if (node._icon != null)
        {
            if (node._icon.style.visibility != 'hidden')
            {
                changeState = true;
            }
            node._icon.style.visibility = 'hidden';
        }
        if (node._shadow != null)
            node._shadow.style.visibility = 'hidden';
        node.closePopup();
        return changeState;
    };

    Indoor2DMap.prototype.showMarker = function(node)
    {
         //console.log("showMarker() nodeid:"+ node.nodeid +" _icon:" + node._icon);

        var changeState = false;

        if (node._icon != null)
        {
            if (node._icon.style.visibility != 'visible')
            {
                changeState = true;
            }
            node._icon.style.visibility = 'visible';
        }
        if (node._shadow != null)
            node._shadow.style.visibility = 'visible';
        //if (changeState)
        //    node.openPopup();
        return changeState;
    };

    Indoor2DMap.prototype.check_visible_state = function(nodeList, nodeid)
    {
        var visibleState = false;
        // find the exist node
        for	(var index = 0; index < nodeList.length; index++)
        {
            var node = nodeList[index];
            if ( node.nodeid === nodeid )
            {
                visibleState = this.check_node_visible_state(node);
                break;
            }
        }
        return visibleState;
    };

    Indoor2DMap.prototype.check_node_visible_state = function(node)
    {
        var visibleState = false;
        if (node._icon != null)
        {
            if (node._icon.style.visibility == 'visible')
            {
                visibleState = true;
            }
        }
        return visibleState;
    };

	//
	// click handle
	//
	Indoor2DMap.prototype.markclick = function(maketype, markindex)
	{
		//alert("maketype = " + maketype + ", markindex=" + markindex);

		var valueIdx = markindex+1;
		if (maketype === 'anchor')
		{
			this.Anchors[markindex].openPopup();
            this.mMap.setView(this.Anchors[markindex].getLatLng());
            //this.mMap.setView(this.Anchors[markindex].getLatLng(), this.popupZoom);
			$("#ProfileMarkerName").text(this.Anchors[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+this.Anchors[markindex].posX+","+this.Anchors[markindex].posY+","+this.Anchors[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(this.Anchors[markindex].date).local().format("MMM Do, hh:mm A"));
 		}
		else
		if (maketype === 'tag')
		{
			this.Tags[markindex].openPopup();
            this.mMap.setView(this.Tags[markindex].getLatLng());
            //this.mMap.setView(this.Tags[markindex].getLatLng(), this.popupZoom);
			$("#ProfileMarkerName").text(this.Tags[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+this.Tags[markindex].posX+","+this.Tags[markindex].posY+","+this.Tags[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(this.Tags[markindex].date).local().format("MMM Do, hh:mm A"));
 		}
        else
		if (maketype === 'Origin')
		{
            if (this.OriginMarker != null)
            {
                this.OriginMarker.openPopup();
                this.mMap.setView(this.OriginMarker.getLatLng());
                //this.mMap.setView(this.OriginMarker.getLatLng(), this.popupZoom);
    			$("#ProfileMarkerName").text(this.OriginMarker.nodename);
			    $("#ProfileMarkerPosition").text("("+this.OriginMarker.posX+","+this.OriginMarker.posY+","+this.OriginMarker.posZ+")");
			    $("#ProfileMarkerLocation").text("");
                $("#ProfileMarkerUpdateTime").text("");
            }
 		}
        else
        if (maketype === 'coord')
		{
			this.Coords[markindex].openPopup();
            this.mMap.setView(this.Coords[markindex].getLatLng());
            //this.mMap.setView(this.Coords[markindex].getLatLng(), this.popupZoom);
			$("#ProfileMarkerName").text(this.Coords[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+this.Coords[markindex].posX+","+this.Coords[markindex].posY+","+this.Coords[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(this.Coords[markindex].date).local().format("MMM Do, hh:mm A"));
		}
		else
        if (maketype === 'locator')
		{
			this.Locators[markindex].openPopup();
            this.mMap.setView(this.Locators[markindex].getLatLng());
            //this.mMap.setView(this.Locators[markindex].getLatLng(), this.popupZoom);
			$("#ProfileMarkerName").text(this.Locators[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+this.Locators[markindex].posX+","+this.Locators[markindex].posY+","+this.Locators[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(this.Locators[markindex].date).local().format("MMM Do, hh:mm A"));
		}

	};

    Indoor2DMap.prototype.UserMapEditControl = L.Control.extend({

        options: {
            position: 'topleft'
        },

        onAdd: function (cur_map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/edit-icon.png)";
            container.style.backgroundSize = "28px 28px";
            container.style.width = '32px';
            container.style.height = '32px';

            container.onclick = function()
            {
                if (cur_map.orig_this.mEdit_mode)
                {
                    cur_map.removeControl(cur_map.orig_this.mUsermapScaleUp);
                    cur_map.removeControl(cur_map.orig_this.mUsermapScaleDown);
                    cur_map.removeControl(cur_map.orig_this.mDrawControl);
                    cur_map.orig_this.mEdit_mode = false;
                    $('#areaname_edit').css('display','none');

                    cur_map.orig_this.setMakerDragable(cur_map.orig_this.mEdit_mode);
                    if (typeof cur_map.orig_this.mEditModeCallback === 'function')
                        cur_map.orig_this.mEditModeCallback(false);
                }
                else
                {
                    cur_map.addControl(cur_map.orig_this.mUsermapScaleUp);
                    cur_map.addControl(cur_map.orig_this.mUsermapScaleDown);
                    cur_map.orig_this.mDrawControl = cur_map.orig_this.creat_drawControl(cur_map.orig_this);
                    cur_map.addControl(cur_map.orig_this.mDrawControl);
                    cur_map.orig_this.mEdit_mode = true;

                    cur_map.orig_this.setMakerDragable(cur_map.orig_this.mEdit_mode);
                    if (typeof cur_map.orig_this.mEditModeCallback === 'function')
                        cur_map.orig_this.mEditModeCallback(true);
                }
            }
            return container;
        }
    });

    Indoor2DMap.prototype.UserMapScaleUpControl = L.Control.extend({
        options: {
            position: 'topleft'
        },

        onAdd: function (cur_map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/maximize-25.png)";
            container.style.backgroundSize = "28px 28px";
            container.style.width = '32px';
            container.style.height = '32px';
            container.style.backgroundRepeat = "no-repeat";

            container.onclick = function()
            {
                cur_map.orig_this.mMap_scale -= cur_map.orig_this.mMap_scale * 0.01;

                cur_map.orig_this.update_2d_map(cur_map.orig_this);

                if (typeof cur_map.orig_this.mUpdateMapInfoCallback === 'function')
                    cur_map.orig_this.mUpdateMapInfoCallback(cur_map.orig_this.mMapLayerId, cur_map.orig_this.mMap_scale, cur_map.orig_this.mMap_coeff);
            }
            return container;
        }
    });

    Indoor2DMap.prototype.UserMapScaleDownControl = L.Control.extend({
        options: {
            position: 'topleft'
        },

        onAdd: function (cur_map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/minimize-25.png)";
            container.style.backgroundSize = "28px 28px";
            container.style.width = '32px';
            container.style.height = '32px';
            container.style.backgroundRepeat = "no-repeat";

            container.onclick = function()
            {
                cur_map.orig_this.mMap_scale += cur_map.orig_this.mMap_scale * 0.01;

                cur_map.orig_this.update_2d_map(cur_map.orig_this);

                if (typeof cur_map.orig_this.mUpdateMapInfoCallback === 'function')
                    cur_map.orig_this.mUpdateMapInfoCallback(cur_map.orig_this.mMapLayerId, cur_map.orig_this.mMap_scale, cur_map.orig_this.mMap_coeff);
            }
            return container;
        }
    });

    // Initialize the draw control and pass it the FeatureGroup of editable layers
	Indoor2DMap.prototype.drawControl = new L.Control.Draw({
		position: 'topleft',
		draw: {
	        polygon: {
	            allowIntersection: false, // Restricts shapes to simple polygons
	            drawError: {
	                color: '#e1e100', // Color the shape will turn when intersects
	                message: '<strong>Oh snap!<strong> you can\'t draw that!' // Message that will show when intersect
	            },
	            shapeOptions: {
	                color: '#bada55'
	            }
	        },
	        rectangle: {
	            shapeOptions: {
	                clickable: true,
	                weight: 1
	            }
	        },
	        polyline: false, // Turns off this drawing tool
	        circle: false, // Turns off this drawing tool
 	        marker: false, // Turns off this drawing tool
            circlemarker: false // Turns off this drawing tool
    	},
	    edit: {
	        featureGroup: this.layer_areas
	    }
	});

    Indoor2DMap.prototype.creat_drawControl = function(orig_this)
    {
        return new L.Control.Draw({
    		position: 'topleft',
    		draw: {
    	        polygon: {
    	            allowIntersection: false, // Restricts shapes to simple polygons
    	            drawError: {
    	                color: '#e1e100', // Color the shape will turn when intersects
    	                message: '<strong>Oh snap!<strong> you can\'t draw that!' // Message that will show when intersect
    	            },
    	            shapeOptions: {
    	                color: '#bada55'
    	            }
    	        },
    	        rectangle: {
    	            shapeOptions: {
    	                clickable: true,
    	                weight: 1
    	            }
    	        },
    	        polyline: false, // Turns off this drawing tool
    	        circle: false, // Turns off this drawing tool
     	        marker: false, // Turns off this drawing tool
                circlemarker: false // Turns off this drawing tool
        	},
    	    edit: {
    	        featureGroup: orig_this.layer_areas
    	    }
    	});
    };

    Indoor2DMap.prototype.setMakerDragable = function(dragable)
    {
        var layer_origin = this.getCurrentLayer("Origin", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_origin) )
        {
            if ( this.OriginMarker != null && this.OriginMarker.dragging != null)
            {
                if (dragable)
                    this.OriginMarker.dragging.enable();
                else
                    this.OriginMarker.dragging.disable();
            }
        }

        var layer_anchors = this.getCurrentLayer("Anchors", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_anchors) )
        {
            for	(var index = 0; index < this.Anchors.length; index++)
            {
                var anchor_node = this.Anchors[index];
                if ( anchor_node == null || anchor_node.dragging == null)
                    continue;
                if (dragable)
                    anchor_node.dragging.enable();
                else
                    anchor_node.dragging.disable();
            }
        }

        var layer_tags = this.getCurrentLayer("Tags", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_tags) )
        {
            for	(var index = 0; index < this.Tags.length; index++)
            {
                var tag_node = this.Tags[index];
                if ( tag_node == null || tag_node.dragging == null)
                    continue;
                if (dragable)
                    tag_node.dragging.enable();
                else
                    tag_node.dragging.disable();
            }
        }

        var layer_coord = this.getCurrentLayer("Coordinators", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_coord) )
        {
            for	(var index = 0; index < this.Coords.length; index++)
            {
                coord_node = this.Coords[index];
                if ( coord_node == null || coord_node.dragging == null)
                    continue;
                if (dragable)
                    coord_node.dragging.enable();
                else
                    coord_node.dragging.disable();
            }
        }

        var layer_locator = this.getCurrentLayer("Locators", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_locator) )
        {
            for	(var index = 0; index < this.Locators.length; index++)
            {
                locator_node = this.Locators[index];
                if ( locator_node == null || locator_node.dragging == null)
                    continue;
                if (dragable)
                    locator_node.dragging.enable();
                else
                    locator_node.dragging.disable();
            }
        }

        var layer_sensor = this.getCurrentLayer("Sensors", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_sensor) )
        {
            for	(var index = 0; index < this.Sensors.length; index++)
            {
                sensor_node = this.Sensors[index];
                if ( sensor_node == null || sensor_node.dragging == null)
                    continue;
                if (dragable)
                    sensor_node.dragging.enable();
                else
                    sensor_node.dragging.disable();
            }
        }

        var layer_user = this.getCurrentLayer("Users", this.mMapLayerId);
        if ( this.mMap.hasLayer(layer_user) )
        {
            for	(var index = 0; index < this.Users.length; index++)
            {
                user_node = this.Users[index];
                if ( user_node == null || user_node.dragging == null)
                    continue;
                if (dragable)
                    user_node.dragging.enable();
                else
                    user_node.dragging.disable();
            }
        }

    };

    Indoor2DMap.prototype.create_maparea_from_geoJSON = function(orig_this, index, geoJSON)
    {
        L.geoJson(geoJSON, {
            pointToLayer: function(feature, latlng)
            {
                //console.log("pointToLayer feature:"+feature+", latlng:"+latlng);
                // var index;
                // for(index in feature)
                // {
                //     var item = feature[index];
                //     console.log("create_maparea_from_geoJSON() feature["+index+"]");
                // }
                return;
            },
            style: function (feature) {
                //console.log("onEachFeature style:"+feature.properties.style);
                return feature.properties.style;
            },
            onEachFeature: function (feature, layer)
            {
                //console.log("onEachFeature feature:"+feature+", layer:"+layer);
                var content = "<label id=\"maparea_popup_"+ feature.properties.areaid+"\">"+ "區域:"+feature.properties.areaname +"&nbsp;&nbsp;&nbsp;&nbsp;</label>";

                var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:true, maxWidth: "190"})
                          .setContent(content);
                layer.bindPopup(markerpopup);
            }
        }).eachLayer(function(layer)
        {
            layer.index = index;
            layer.orig_this = orig_this;
            layer.on('click', orig_this.mapAreaOnClick);

            orig_this.MapAreas[index] = layer;
            orig_this.layer_areas.addLayer(layer);

            //console.log("MapAreas:[" + index +"]:" + orig_this.MapAreas[index].feature.properties.areaname);
        });
    };

    Indoor2DMap.prototype.load_allmaparea = function(orig_this)
    {
        orig_this.layer_areas.eachLayer(function (layer)
        {
            orig_this.layer_areas.removeLayer(layer);
        });

        var loMapAreas = [];
        //var orig_this = this;

        if (typeof get_maplayerarea === 'function')
        {
            get_maplayerarea(this.mMapLayerId, function(data)
            {
                //alert("load_allmaparea data="+data);

                $.each(data, function(itemindex, area)
                {
                    var geoJSON = JSON.parse(area);
                    loMapAreas[itemindex] = geoJSON;
                });

                //
                // All area load finished!
                //
                var count = 0;
                for(var index in loMapAreas)
                {
                    var geoJSON = loMapAreas[index];

                    if ( geoJSON.properties.areatype === '6') // 病房 最後才  load
                    {
                        // skip
                    }
                    else
                    {
                        orig_this.create_maparea_from_geoJSON(orig_this, count, geoJSON);
                        count++;
                    }
                }

                var index;
                for(index in loMapAreas)
                {
                    var geoJSON = loMapAreas[index];

                    if ( geoJSON.properties.areatype === '6')
                    {
                        orig_this.create_maparea_from_geoJSON(orig_this, count, geoJSON);
                        count++;
                    }
                }
            });
        }
    };

    //
    // load GeoJSON from GAE
    //
    Indoor2DMap.prototype.load_maparea = function()
    {
        this.load_allmaparea(this);
    };

    Indoor2DMap.prototype.create_map_success = function(orig_this, response, area_index)
    {
        var oldlayer = orig_this.MapAreas[area_index];
        orig_this.layer_areas.removeLayer(oldlayer);

        orig_this.create_map_area(orig_this, response['geojson'], area_index);
    };

    Indoor2DMap.prototype.create_map_action = function(map)
    {
        map.on('draw:created', function (e)
    	{
    	    var type = e.layerType,
    	        layer = e.layer;

            layer.options.fillColor = layer.options.color;
            layer.options.fillOpacity = layer.options.opacity;
            layer.options.color = map.orig_this.areaborder_color;
            layer.options.opacity = map.orig_this.areaborder_opacity;

            layer.index = Object.keys(map.orig_this.MapAreas).length;
            layer.orig_this = map.orig_this;
            layer.on('click', map.orig_this.mapAreaOnClick);
            map.orig_this.layer_areas.addLayer(layer);

    	    var shape = layer.toGeoJSON();
            // add style
    		//alert("draw:created layer.options:"+ JSON.stringify(layer.options));
    		shape.properties.style = {};
            shape.properties.style.color = layer.options.color;
            shape.properties.style.opacity = layer.options.opacity;
            shape.properties.style.weight = layer.options.weight;
            shape.properties.style.fillColor = getRandomColor();
            shape.properties.style.fillOpacity = layer.options.fillOpacity;

            // get random area id
            shape.properties.areaid = "area" + getRandomInt(1000, 9999);
            shape.properties.areaname = "Area " + Object.keys(map.orig_this.MapAreas).length;

            //MapAreas[shape.properties.areaid] = layer;
            map.orig_this.MapAreas[layer.index] = layer;

    	    var shape_for_db = JSON.stringify(shape);
    	    //alert("draw:created shape_for_db:"+shape_for_db);

    		//
    		// create new GeoJSON
    		//
            var area_type = 0;
    		create_maplayerarea(map.orig_this, map.orig_this.mMapLayerId, shape_for_db, shape.properties.areaid, shape.properties.areaname, area_type, layer.index, map.orig_this.create_map_success);
    	});

    	map.on('draw:edited', function (e)
    	{
    	    // Update db to save latest changes.
    	    var layers = e.layers;
        	layers.eachLayer(function (layer)
        	{
    	        var shape = layer.toGeoJSON();
    	        var area_id = shape.properties.areaid;
                var area_name= shape.properties.areaname;
                var shape_for_db = JSON.stringify(shape);
                //alert("draw:edited shape_for_db:"+shape_for_db);

     	    	// console.log("draw:edited layer = " + layer.constructor.name +", layer.getLatLngs() = " + layer.getLatLngs());
                // console.log("draw:edited area_id = " + area_id +", area_name = " + area_name);

       			//
       			// update GeoJSON
       			//
                update_maplayerarea_shape(map.orig_this.mMapLayerId, area_id, shape_for_db, shape.properties.areaname);
        	});

    	});

    	map.on('draw:deletestart', function (e)
    	{
    	   map.orig_this.mEdit_delete_mode = true;
    	});

    	map.on('draw:deletestop', function (e)
    	{
    	   map.orig_this.mEdit_delete_mode = false;
    	});

    	map.on('draw:deleted', function (e)
    	{
    	    // Update db to save latest changes.
    	    var layers = e.layers;
        	layers.eachLayer(function (layer)
        	{
     	        var shape = layer.toGeoJSON();
    	        var area_id = shape.properties.areaid;
                //alert("draw:deleted area_id:"+area_id);
    	        map.orig_this.delete_map_area(map.orig_this, area_id);
        	});
            map.orig_this.mEdit_delete_mode = false;
    	});

    	map.on('moveend', function ()
    	{
    	     //var map_center = L.latLng(0.00032, 0.00049);
    	     //map.setView(map_center);
    	});
    };

	//
	// delete a map area
	//
	Indoor2DMap.prototype.delete_map_area = function(orig_this, area_id)
	{
        //alert("delete_map_area area_id:"+area_id);
        //delete_map_area_from_center(area_id);
        delete_maplayerarea(orig_this.mMapLayerId, area_id);
    };

    //
    // create map area from GeoJSON
    //
	Indoor2DMap.prototype.create_map_area = function(orig_this, geojson, area_index)
	{
        //alert("create_map_area:geojson="+geojson);

        return L.geoJson(JSON.parse(geojson), {
            pointToLayer: function(feature, latlng)
            {
                return;
            },
            style: function (feature) {
                 return feature.properties.style;
            },
            onEachFeature: function (feature, layer) {
							var showName = feature.properties.areaname + "&nbsp;&nbsp;&nbsp;&nbsp;<br>&nbsp;&nbsp;";
	      	    //alert("create_map_area:showName = "+showName);
                layer.bindPopup(showName);
            }
        }).eachLayer(function(layer)
        {
            layer.index = area_index;
            layer.orig_this = orig_this;
            layer.on('click', orig_this.mapAreaOnClick);
            orig_this.MapAreas[area_index] = layer;
            orig_this.layer_areas.addLayer(layer);
        });
	};

    Indoor2DMap.prototype.areacolorOnInput = function()
    {
        var areacolor = $('#areacolor_input').val();

        if (areacolor.charAt(0) != '#')
        {
            console.log("areacolorOnInput() not valid color!");
            return;
        }
        if (areacolor.length != 9)
        {
            console.log("areacolorOnInput() not valid color string length! need 9, get " + areacolor.length);
            return;
        }

        var fillColor = areacolor.substring(0, 7);
        var OpacityString = '0x' + areacolor.substring(7, 9);
        var OpacityInt = parseInt(OpacityString);
        var OpacityFloat = parseFloat(OpacityInt);
        var fillOpacity = (OpacityFloat / 255.0);

        //console.log("areacolorOnInput() fillColor:"+fillColor);
        //console.log("areacolorOnInput() OpacityString:"+OpacityString +", OpacityInt:"+OpacityInt + ", fillOpacity:"+fillOpacity);

        $('#areaname_color').minicolors('value', fillColor);
        $('#areaname_color').minicolors('opacity', fillOpacity);

    };

    Indoor2DMap.prototype.toggle_map_area_editor = function(shape, itemindex)
    {
        this.mEdit_maparea_index = itemindex;
        $('#areaname_input').val(shape.properties.areaname);
        $("#area_type_show").html(convert_area_type_name(shape.properties.areatype));
        $("#area_type_show").attr("data-id", shape.properties.areatype);
        $('#areaname_color').minicolors('value', shape.properties.style.fillColor);
        $('#areaname_color').minicolors('opacity', shape.properties.style.fillOpacity);

        //console.log("toggle_map_area_editor() fillColor:" + shape.properties.style.fillColor);
        //console.log("toggle_map_area_editor() fillOpacity:" + shape.properties.style.fillOpacity);

        var Opacity = parseInt(shape.properties.style.fillOpacity * 255);
        var hex = Opacity.toString(16);
        //console.log("toggle_map_area_editor() Opacity hex:" + hex);
        $('#areacolor_input').val(shape.properties.style.fillColor + hex );

        var areacolor_input = document.getElementById("areacolor_input");
        areacolor_input.addEventListener('input', this.areacolorOnInput);


        $('#areaname_edit').css('display','block');
    };

     Indoor2DMap.prototype.areaname_save = function(orig_this)
     {
         var hex = $('#areaname_color').minicolors('value');
         var opacity = $('#areaname_color').minicolors('opacity');
         var areaname = $('#areaname_input').val();
         var areatype = $("#area_type_show").attr("data-id");

         var layer = orig_this.MapAreas[orig_this.mEdit_maparea_index];
         layer.setStyle({color:orig_this.areaborder_color, opacity:orig_this.areaborder_opacity, fillColor:hex, fillOpacity:opacity});
         layer.bindPopup(areaname + "&nbsp;&nbsp;&nbsp;&nbsp;<br>");

         $('#areaname_edit').css('display','none');

         var shape = layer.toGeoJSON();

         shape.properties.areaname = areaname;
         shape.properties.areatype = areatype;
         shape.properties.style.color = orig_this.areaborder_color;
         shape.properties.style.opacity = orig_this.areaborder_opacity;
         shape.properties.style.fillColor = hex;
         shape.properties.style.fillOpacity = opacity;

         var area_id = shape.properties.areaid;

         var shape_for_db = JSON.stringify(shape);

         //console.log("draw:edited shape_for_db:"+shape_for_db);

 		//
 		// update GeoJSON
 		//
        update_maplayerarea(orig_this.mMapLayerId, area_id, shape_for_db, areaname, areatype);
    };

    //
    // for draw tag
    //
    Indoor2DMap.prototype.push_to_drawlist = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
    {
        // find the exist node
        var found = false;

		//console.log("push_to_drawlist() nodeid("+nodeid+")");

        for	(var index = 0; index < this.TagDrawList.length; index++)
        {
            var exist_node = this.TagDrawList[index];
            if ( exist_node.nodeid === nodeid )
            {
                if ( exist_node.preposX == 0 && exist_node.preposY == 0 && exist_node.preposZ == 0 )
                {
                    exist_node.totalposX = parseInt(posX);
                    exist_node.totalposY = parseInt(posY);
                    exist_node.totalposZ = parseInt(posZ);
                    exist_node.count = 1;
                }
                else
                {
                    exist_node.totalposX += parseInt(posX);
                    exist_node.totalposY += parseInt(posY);
                    exist_node.totalposZ += parseInt(posZ);
                    exist_node.count++;
                }
                exist_node.buildingid = buildingid;
                exist_node.floor = floor;
                exist_node.date = date;
                exist_node.hide = false;

                // if (nodeid == "Tag0013")
                // {
                //     console.log("push_to_drawlist() " + nodeid+" ("+posX+","+posY+","+posZ+") floor:"+floor+" prepos("+exist_node.preposX+","+exist_node.preposY+","+exist_node.preposZ+")"+" totalpos("+exist_node.totalposX+","+exist_node.totalposY+","+exist_node.totalposZ+") count:"+exist_node.count);
                // }

                found = true;
            }
        }
        if (found == false)
        {
            // if (nodeid == "Tag011")
            // {
            //     console.log("push_to_drawlist() found == false");
            // }
            var newNode = {};
            newNode.nodeid = nodeid;
            newNode.macaddress = macaddress;
            newNode.nodename = nodename;
            newNode.date = date;
            this.reset_draw_node(newNode, posX, posY, posZ, buildingid, floor);

            this.TagDrawList.push(newNode);
        }
    };

    Indoor2DMap.prototype.hide_2d_drawlist = function(nodeid)
    {
        for	(var index = 0; index < this.TagDrawList.length; index++)
        {
            var exist_node = this.TagDrawList[index];
            if ( exist_node.nodeid === nodeid )
            {
                // console.log("hide_2d_drawlist() nodeid:"+nodeid);

                exist_node.hide = true;
                break;
            }
        }
    };

    Indoor2DMap.prototype.hide_2d_drawGPSlist = function(nodeid)
    {
        for	(var index = 0; index < this.TagGPSDrawList.length; index++)
        {
            var exist_node = this.TagGPSDrawList[index];
            if ( exist_node.nodeid === nodeid )
            {
                // console.log("hide_2d_drawlist() nodeid:"+nodeid);

                exist_node.hide = true;
                break;
            }
        }
    };

    Indoor2DMap.prototype.reset_node = function(nodeid, posX, posY, posZ, buildingid, floor)
    {
        for	(var index = 0; index < this.TagDrawList.length; index++)
        {
            var exist_node = this.TagDrawList[index];
            if ( exist_node.nodeid === nodeid )
            {
                this.reset_draw_node(exist_node, posX, posY, posZ, buildingid, floor);
                break;
            }
        }
    };

    Indoor2DMap.prototype.reset_draw_node = function(exist_node, posX, posY, posZ, buildingid, floor)
    {
        exist_node.buildingid = buildingid;
        exist_node.floor = floor;
        exist_node.count = 0;
        exist_node.totalposX = 0;
        exist_node.totalposY = 0;
        exist_node.totalposZ = 0;
        exist_node.preAveragePosX = parseInt(posX);
        exist_node.preAveragePosY = parseInt(posY);
        exist_node.preAveragePosZ = parseInt(posZ);
        exist_node.preposX = parseInt(posX);
        exist_node.preposY = parseInt(posY);
        exist_node.preposZ = parseInt(posZ);
        exist_node.curposX = parseInt(posX);
        exist_node.curposY = parseInt(posY);
        exist_node.curposZ = parseInt(posZ);
        //newNode.start_show_tag_moving = 0;
        exist_node.draw_loop_index = 0;
        exist_node.not_move_count = 0;
        exist_node.move_no_block_X = parseInt(posX);
        exist_node.move_no_block_old_X = parseInt(posX);
        exist_node.move_moving_X = parseInt(posX);
        exist_node.move_moving_old_X = parseInt(posX);
        exist_node.move_no_block_Y = parseInt(posY);
        exist_node.move_no_block_old_Y = parseInt(posY);
        exist_node.move_moving_Y = parseInt(posY);
        exist_node.move_moving_old_Y = parseInt(posY);
        exist_node.move_status = 1;
        exist_node.alertcircle = null;
        exist_node.alertcount = 0;
        exist_node.alertType = -1;
        exist_node.hide = false;
    };

    //
    // for draw GPS tag
    //
    Indoor2DMap.prototype.push_to_GPSdrawlist = function(nodeid, Longitude, Latitude, buildingid, floor)
    {
        // find the exist node
        var found = false;
		// console.log("push_to_GPSdrawlist() nodeid("+nodeid+")" + ", Longitude:"+Longitude+", Latitude:"+Latitude);

        for	(var index = 0; index < this.TagGPSDrawList.length; index++)
        {
            var exist_node = this.TagGPSDrawList[index];
            if ( exist_node.nodeid === nodeid )
            {
                if ( exist_node.preposLng == 0 && exist_node.preposLat == 0)
                {
                    exist_node.totalposLng = parseFloat(Longitude);
                    exist_node.totalposLat = parseFloat(Latitude);
                    exist_node.count = 1;
                }
                else
                {
                    exist_node.totalposLng += parseFloat(Longitude);
                    exist_node.totalposLat += parseFloat(Latitude);
                    exist_node.count++;
                }

                exist_node.hide = false;
                found = true;
            }
        }
        if (found == false)
        {
            var newNode = {};
            newNode.nodeid = nodeid;
            this.reset_GPSdraw_node(newNode, Longitude, Latitude, buildingid, floor);

            this.TagGPSDrawList.push(newNode);
        }
    };

    Indoor2DMap.prototype.reset_GPSdraw_node = function(exist_node, Longitude, Latitude, buildingid, floor)
    {
        exist_node.buildingid = buildingid;
        exist_node.floor = floor;
        exist_node.count = 0;
        exist_node.totalposLng = 0.0;
        exist_node.totalposLat = 0.0;
        exist_node.preAveragePosLng = parseFloat(Longitude);
        exist_node.preAveragePosLat = parseFloat(Latitude);
        exist_node.preposLng = parseFloat(Longitude);
        exist_node.preposLat = parseFloat(Latitude);
        exist_node.curposLng = parseFloat(Longitude);
        exist_node.curposLat = parseFloat(Latitude);
        exist_node.draw_loop_index = 0;
        exist_node.not_move_count = 0;
        exist_node.move_no_block_Lng = parseFloat(Longitude);
        exist_node.move_no_block_old_Lng = parseFloat(Longitude);
        exist_node.move_moving_Lng = parseFloat(Longitude);
        exist_node.move_moving_old_Lng = parseFloat(Longitude);
        exist_node.move_no_block_Lat = parseFloat(Latitude);
        exist_node.move_no_block_old_Lat = parseFloat(Latitude);
        exist_node.move_moving_Lat = parseFloat(Latitude);
        exist_node.move_moving_old_Lat = parseFloat(Latitude);
        exist_node.move_status = 1;
        exist_node.alertcircle = null;
        exist_node.alertcount = 0;
        exist_node.alertType = -1;
    };

	Indoor2DMap.prototype.draw_tag_smooth_move = function(orig_this)
    {
        if (orig_this.mEdit_mode)
        {
            setTimeout(orig_this.draw_tag_smooth_move, 1000, orig_this);
            return;
        }

        for	(var index = 0; index < orig_this.TagDrawList.length; index++)
        {
            var exist_node = orig_this.TagDrawList[index];

            if ( exist_node.hide == true)
            {
                //console.log("draw_tag_smooth_move() exist_node.hide nodeid:"+exist_node.nodeid);
                continue;
            }

            // if (exist_node.nodeid == 'Tag0013')
            // {
            //     console.log("draw_tag_smooth_move() show nodeid:"+exist_node.nodeid);
            // }

            var preposX = exist_node.preposX;
            var preposY = exist_node.preposY;
            var preposZ = exist_node.preposZ;
            var curposX = exist_node.curposX;
            var curposY = exist_node.curposY;
            var curposZ = exist_node.curposZ;

            var total_distance_X = curposX - preposX;
            var total_distance_Y = curposY - preposY;
            var total_distance_Z = curposZ - preposZ;

            var distance = Math.sqrt(total_distance_X * total_distance_X + total_distance_Y * total_distance_Y + total_distance_Z * total_distance_Z);

            var showPosX = curposX;
            var showPosY = curposY;
            var showPosZ = curposZ;

    		if ( distance >= 5 || exist_node.not_move_count > 10)
            {
                var current_time = exist_node.draw_loop_index + 1;

                var step_dist_x = total_distance_X / orig_this.move_step_size;
                var step_dist_y = total_distance_Y / orig_this.move_step_size;
                var step_dist_z = total_distance_Z / orig_this.move_step_size;

                if (current_time >= orig_this.move_step_size)
                    current_time = orig_this.move_step_size;

                var dist_x = parseInt(step_dist_x * current_time);
                var dist_y = parseInt(step_dist_y * current_time);
                var dist_z = parseInt(step_dist_z * current_time);

                exist_node.not_move_count = 0;

                if (orig_this.move_loop_count < 10)
                {
                    // first load map , does not show.
                }
                else
                {
                    showPosX = exist_node.preposX + dist_x;
                    showPosY = exist_node.preposY + dist_y;
                    showPosZ = exist_node.preposZ + dist_z;

                    orig_this.updateMarker(orig_this.Tags, exist_node.macaddress, exist_node.nodeid, exist_node.nodename,
                        showPosX, showPosY, showPosZ,
                        exist_node.buildingid, exist_node.floor, exist_node.date, true, true);
                }
            }
            else
            {
                exist_node.not_move_count++;
            }

            exist_node.draw_loop_index++;

            if (exist_node.draw_loop_index >= orig_this.move_step_size)
            {
                if (exist_node.count > 0)
                {
                    // average the values
                    //
                    var posX = parseInt(exist_node.totalposX / exist_node.count);
                    var posY = parseInt(exist_node.totalposY / exist_node.count);
                    var posZ = parseInt(exist_node.totalposZ / exist_node.count);

                    r = 1.0;

                    // var r = getCurrent_r(exist_node.move_status);
                    //
                    // if ( r == 0.1)
                    // {
                    //     smoothing_time = 200;// ms
                    // }
                    // else
                    // if ( r == 0.4)
                    // {
                    //     smoothing_time = 600;// ms
                    // }
                    // else
                    // {
                    //     smoothing_time = 800;// ms
                    // }
					// if (exist_node.start_show_tag_moving == 0)
					// {
					// 	r = 1;
					// }

                    exist_node.move_moving_X = parseInt(r * (posX - exist_node.move_no_block_old_X));
                    exist_node.move_no_block_X = exist_node.move_moving_X + exist_node.move_no_block_old_X;
                    exist_node.move_moving_old_X = exist_node.move_moving_X;

					exist_node.move_moving_Y = parseInt(r * (posY - exist_node.move_no_block_old_Y));
                    exist_node.move_no_block_Y = exist_node.move_moving_Y + exist_node.move_no_block_old_Y;
                    exist_node.move_moving_old_Y = exist_node.move_moving_Y;

                    exist_node.move_status += 1;
                    if ( exist_node.move_status > 6 || ((exist_node.move_moving_X * exist_node.move_moving_old_X) < 0))
                    {
                        exist_node.move_status = 1;
                    }

                    exist_node.move_no_block_old_X = exist_node.move_no_block_X;
					exist_node.move_no_block_old_Y = exist_node.move_no_block_Y;
                    //========================

                    exist_node.preposX = exist_node.curposX;
                    exist_node.preposY = exist_node.curposY;
                    exist_node.preposZ = exist_node.curposZ;
                    exist_node.curposX = exist_node.move_no_block_X;
                    exist_node.curposY = exist_node.move_no_block_Y;
                    exist_node.curposZ = posZ;
                    exist_node.preAveragePosX = parseInt(exist_node.totalposX / exist_node.count);
                    exist_node.preAveragePosY = parseInt(exist_node.totalposY / exist_node.count);
                    exist_node.preAveragePosZ = parseInt(exist_node.totalposZ / exist_node.count);
					exist_node.totalposX = 0;
                    exist_node.totalposY = 0;
                    exist_node.totalposZ = 0;
                    exist_node.count = 0;
                    exist_node.draw_loop_index = 0;
                }

                if (exist_node.draw_loop_index > orig_this.move_step_size*2)
                {
                    // finished!
                    exist_node.preposX = exist_node.curposX;
                    exist_node.preposY = exist_node.curposY;
                    exist_node.preposZ = exist_node.curposZ;
                    exist_node.draw_loop_index = 0;
                }
            }
        }
        orig_this.move_loop_count++;
        if (orig_this.move_loop_count >= 10000)
            orig_this.move_loop_count = 100;

        if (orig_this.mEnableUISmoothing)
            setTimeout(orig_this.draw_tag_smooth_move, orig_this.smoothing_time/orig_this.move_step_size, orig_this);
    };

	Indoor2DMap.prototype.draw_tag_GPS_smooth_move = function(orig_this)
    {
        if (orig_this.mEdit_mode)
        {
            setTimeout(orig_this.draw_tag_GPS_smooth_move, 1000, orig_this);
            return;
        }

        for	(var index = 0; index < orig_this.TagGPSDrawList.length; index++)
        {
            var exist_node = orig_this.TagGPSDrawList[index];

            if ( exist_node.hide == true)
            {
                // console.log("draw_tag_GPS_smooth_move() nodeid:"+exist_node.nodeid);
                continue;
            }

            var preposLng = exist_node.preposLng;
            var preposLat = exist_node.preposLat;
            var curposLng = exist_node.curposLng;
            var curposLat = exist_node.curposLat;

            var total_distance_Lng = curposLng - preposLng;
            var total_distance_Lat = curposLat - preposLat;

            var distance = Math.sqrt(total_distance_Lng * total_distance_Lng + total_distance_Lat * total_distance_Lat);

            var showPosLng = curposLng;
            var showPosLat = curposLat;

    		if ( exist_node.not_move_count > 10)
            {
                var current_time = exist_node.draw_loop_index + 1;

                var step_dist_Lng = total_distance_Lng / orig_this.GPS_move_step_size;
                var step_dist_Lat = total_distance_Lat / orig_this.GPS_move_step_size;

                //console.log(exist_node.nodeid +", total_distance_Lng:"+total_distance_Lng+", total_distance_Lat:"+total_distance_Lat)
                //console.log(exist_node.nodeid +", step_dist_Lng:"+step_dist_Lng+", step_dist_Lat:"+step_dist_Lat)

                if (current_time >= orig_this.GPS_move_step_size)
                    current_time = orig_this.GPS_move_step_size;

                var dist_Lng = parseFloat(step_dist_Lng * current_time);
                var dist_Lat = parseFloat(step_dist_Lat * current_time);

                //console.log(exist_node.nodeid +", dist_Lng:"+dist_Lng+", dist_Lat:"+dist_Lat)

                exist_node.not_move_count = 0;

                if (orig_this.GPS_move_loop_count < 10)
                {
                    // first load map , does not show.
                }
                else
                {

                    //console.log(exist_node.nodeid +", preposLng:"+exist_node.preposLng+", preposLat:"+exist_node.preposLat)

                    showPosLng = exist_node.preposLng + dist_Lng;
                    showPosLat = exist_node.preposLat + dist_Lat;

                    orig_this.updateTagGPS(exist_node.nodeid, showPosLng, showPosLat);
                    //console.log(exist_node.nodeid +", showPosLng:"+showPosLng+", showPosLat:"+showPosLat)
                }
            }
            else
            {
                exist_node.not_move_count++;
            }

            exist_node.draw_loop_index++;

            if (exist_node.draw_loop_index >= orig_this.GPS_move_step_size)
            {
                if (exist_node.count > 0)
                {
                    // average the values
                    //
                    var posLng = parseFloat(exist_node.totalposLng / exist_node.count);
                    var posLat = parseFloat(exist_node.totalposLat / exist_node.count);

                    // r = 1.0;

                    var r = getCurrent_r(exist_node.move_status);

                    if ( r == 0.1)
                    {
                        orig_this.GPS_smoothing_time = 200;// ms
                    }
                    else
                    if ( r == 0.4)
                    {
                        orig_this.GPS_smoothing_time = 400;// ms
                    }
                    else
                    {
                        orig_this.GPS_smoothing_time = 800;// ms
                    }
					// if (exist_node.start_show_tag_moving == 0)
					// {
					// 	r = 1;
					// }

                    exist_node.move_moving_Lng = parseFloat(r * (posLng - exist_node.move_no_block_old_Lng));
                    exist_node.move_no_block_Lng = exist_node.move_moving_Lng + exist_node.move_no_block_old_Lng;
                    exist_node.move_moving_old_Lng = exist_node.move_moving_Lng;

					exist_node.move_moving_Lat = parseFloat(r * (posLat - exist_node.move_no_block_old_Lat));
                    exist_node.move_no_block_Lat = exist_node.move_moving_Lat + exist_node.move_no_block_old_Lat;
                    exist_node.move_moving_old_Lat = exist_node.move_moving_Lat;

                    exist_node.move_status += 1;
                    if ( exist_node.move_status > 6 || ((exist_node.move_moving_Lng * exist_node.move_moving_old_Lng) < 0))
                    {
                        exist_node.move_status = 1;
                    }

                    exist_node.move_no_block_old_Lng = exist_node.move_no_block_Lng;
					exist_node.move_no_block_old_Lat = exist_node.move_no_block_Lat;
                    //========================

                    exist_node.preposLng = exist_node.curposLng;
                    exist_node.preposLat = exist_node.curposLat;
                    exist_node.curposLng = exist_node.move_no_block_Lng;
                    exist_node.curposLat = exist_node.move_no_block_Lat;
                    exist_node.preAveragePosX = parseFloat(exist_node.totalposLng / exist_node.count);
                    exist_node.preAveragePosY = parseFloat(exist_node.totalposLat / exist_node.count);
                    exist_node.totalposLng = 0;
                    exist_node.totalposLat = 0;
                    exist_node.count = 0;
                    exist_node.draw_loop_index = 0;
                }

                if (exist_node.draw_loop_index > orig_this.GPS_move_step_size*2)
                {
                    // finished!
                    exist_node.preposLng = exist_node.curposLng;
                    exist_node.preposLat = exist_node.curposLat;
                    exist_node.draw_loop_index = 0;
                }
            }
        }
        orig_this.GPS_move_loop_count++;
        if (orig_this.GPS_move_loop_count >= 10000)
            orig_this.GPS_move_loop_count = 100;

        if (orig_this.mEnableUISmoothing)
            setTimeout(orig_this.draw_tag_GPS_smooth_move, orig_this.GPS_smoothing_time/orig_this.GPS_move_step_size, orig_this);
    };

    Indoor2DMap.prototype.update_alert_circle = function(orig_this)
    {
        orig_this.update_tag_alert_circle();
        orig_this.update_tag_GPS_alert_circle();
        orig_this.update_locator_alert_circle();

        setTimeout(orig_this.update_alert_circle, 100, orig_this);
    };

    Indoor2DMap.prototype.update_tag_alert_circle = function ()
    {
        for	(var index = 0; index < this.TagDrawList.length; index++)
        {
            var exist_node = this.TagDrawList[index];

            if ( exist_node.hide == true)
            {
                //console.log("draw_tag_smooth_move() exist_node.hide nodeid:"+exist_node.nodeid);
                continue;
            }
            var circle = exist_node.alertcircle;

            if ( circle != undefined && circle != null)
            {
                //console.log("update_tag_alert_circle() nodeid:"+exist_node.nodeid+" circle != undefined && circle != null");

                var opacity = circle.options.opacity;
                var new_opacity = opacity;
                var old_radius = circle.options.radius;
                var new_radius = old_radius;

                //console.log("draw_tag_loop_move() " + exist_node.nodeid + " alert circle opacity="+opacity+", old_radius="+old_radius);

                if ( opacity == 0.2 )
                {
                    // scale down
                    if (old_radius > 5)
                    {
                        new_radius = old_radius - 5;
                    }
                    else
                    {
                        new_opacity = 0.3;
                    }
                }
                else
                {
                    //scale up
                    if (old_radius > 20)
                    {
                        new_opacity = 0.2;
                    }
                    else
                    {
                        new_radius = old_radius + 5;
                    }
                }
                //
                // scle up down icon
                //
                var TargetNode = this.getTargetNode(exist_node.nodeid);

                //
                // update UI
                //
                //var nodeLatLng = convertToLatLng(showPosX, showPosY);
                // console.log("draw_tag_loop_move() " + exist_node.nodeid + ' new_radius:'+new_radius+' new_opacity:'+new_opacity);
                circle.setStyle({radius:new_radius, opacity: new_opacity});
                circle.setLatLng(TargetNode.getLatLng());

                if (this.AlertIconScaleIndexList[exist_node.nodeid] == undefined )
                    this.AlertIconScaleIndexList[exist_node.nodeid] = 0;
                var curAlertScaleIndex = this.AlertIconScaleIndexList[exist_node.nodeid];

                if ( this.NodeAlertFunction[exist_node.nodeid] == 0)
                    TargetNode.setIcon(this.AlertIconScaleUpDownList[0]);
                else
                    TargetNode.setIcon(this.AlertIconScaleUpDownList[curAlertScaleIndex]);

                curAlertScaleIndex++;
                if (curAlertScaleIndex >= 10)
                    curAlertScaleIndex = 0;
                this.AlertIconScaleIndexList[exist_node.nodeid] = curAlertScaleIndex;
            }
        }
    };

    Indoor2DMap.prototype.update_tag_GPS_alert_circle = function()
    {
        for	(var index = 0; index < this.TagGPSDrawList.length; index++)
        {
            var exist_node = this.TagGPSDrawList[index];

            if ( exist_node.hide == true)
            {
                // console.log("draw_tag_GPS_smooth_move() nodeid:"+exist_node.nodeid);
                continue;
            }

            var circle = exist_node.alertcircle;
            if ( circle != undefined && circle != null)
            {
                var opacity = circle.options.opacity;
                var new_opacity = opacity;
                var old_radius = circle.options.radius;
                var new_radius = old_radius;

                //console.log("draw_tag_loop_move() " + exist_node.nodeid + " alert circle opacity="+opacity+", old_radius="+old_radius);

                if ( opacity == 0.2 )
                {
                    // scale down
                    if (old_radius > 5)
                    {
                        new_radius = old_radius - 5;
                    }
                    else
                    {
                        new_opacity = 0.3;
                    }
                }
                else
                {
                    //scale up
                    if (old_radius > 20)
                    {
                        new_opacity = 0.2;
                    }
                    else
                    {
                        new_radius = old_radius + 5;
                    }
                }

                //
                // scle up down icon
                //
                var TargetNode = this.getTargetNode(exist_node.nodeid);

                //
                // update UI
                //
                circle.setStyle({radius:new_radius, opacity: new_opacity});
                circle.setLatLng(TargetNode.getLatLng());

                if (this.AlertIconScaleIndexList[exist_node.nodeid] == undefined )
                    this.AlertIconScaleIndexList[exist_node.nodeid] = 0;
                var curAlertScaleIndex = this.AlertIconScaleIndexList[exist_node.nodeid];
                TargetNode.setIcon(this.AlertIconScaleUpDownList[curAlertScaleIndex]);

                curAlertScaleIndex++;
                if (curAlertScaleIndex >= 10)
                    curAlertScaleIndex = 0;
                this.AlertIconScaleIndexList[exist_node.nodeid] = curAlertScaleIndex;
            }
        }
    };

    Indoor2DMap.prototype.update_locator_alert_circle = function()
    {
        for	(var index = 0; index < this.Locators.length; index++)
        {
            var exist_node = this.Locators[index];

            var circle = exist_node.alertcircle;
            if ( circle != undefined && circle != null)
            {
                var opacity = circle.options.opacity;
                var new_opacity = opacity;
                var old_radius = circle.options.radius;
                var new_radius = old_radius;

                //console.log("draw_tag_loop_move() " + exist_node.nodeid + " alert circle opacity="+opacity+", old_radius="+old_radius);

                if ( opacity == 0.2 )
                {
                    // scale down
                    if (old_radius > 5)
                    {
                        new_radius = old_radius - 5;
                    }
                    else
                    {
                        new_opacity = 0.3;
                    }
                }
                else
                {
                    //scale up
                    if (old_radius > 20)
                    {
                        new_opacity = 0.2;
                    }
                    else
                    {
                        new_radius = old_radius + 5;
                    }
                }
                //
                // scle up down icon
                //
                var TargetNode = this.getTargetLocatorNode(exist_node.nodeid);

                //
                // update UI
                //
                circle.setStyle({radius:new_radius, opacity: new_opacity});
                circle.setLatLng(exist_node.getLatLng());


                if (this.AlertIconScaleIndexList[exist_node.nodeid] == undefined )
                    this.AlertIconScaleIndexList[exist_node.nodeid] = 0;
                var curAlertScaleIndex = this.AlertIconScaleIndexList[exist_node.nodeid];
                TargetNode.setIcon(this.AlertIconScaleUpDownList[curAlertScaleIndex]);

                curAlertScaleIndex++;
                if (curAlertScaleIndex >= 10)
                    curAlertScaleIndex = 0;
                this.AlertIconScaleIndexList[exist_node.nodeid] = curAlertScaleIndex;
            }
        }
    };

    Indoor2DMap.prototype.convertToLatLng = function(orig_this, posX, posY)
    {
        //console.log("convertToLatLng() posX:"+posX+", posY:"+posY);
        var lat = centimeterToLatitude(parseFloat(posY));
        var cen = LatitudeToCentimeter(lat);
        //console.log("convertToLatLng() centimeterToLatitude(parseFloat(posY):"+lat+", cen:"+ cen);
        //console.log("convertToLatLng() mWGS48OriginX:"+this.mWGS48OriginX+", mWGS48OriginY:"+this.mWGS48OriginY);
        //======================
        var NodeLatitude = orig_this.mWGS48OriginY + centimeterToLatitude(parseFloat(posY));
        var NodeLongitude = orig_this.mWGS48OriginX + centimeterToLongitude(NodeLatitude, parseFloat(posX));
        //======================
        //var NodeLatitude = mWGS48OriginY + centimeterTolatLng(parseFloat(posY));
        //console.log("convertToLatLng() 2 NodeLatitude:"+centimeterTolatLng(parseFloat(posY)));
        //var NodeLongitude = mWGS48OriginX + centimeterTolatLng(parseFloat(posX));
        //======================
        //console.log("convertToLatLng() NodeLatitude:"+NodeLatitude+", NodeLongitude:"+NodeLongitude+", mRotateMap:"+this.mRotateMap);
        //
        // var NodeDiff_Y = NodeLatitude - mWGS48OriginY;
        // var NodeDiff_X = NodeLongitude - mWGS48OriginX;
        //
        // console.log("convertToLatLng() NodeDiff_X:"+NodeDiff_X+", NodeDiff_Y:"+NodeDiff_Y);

        if (orig_this.mRotateMap)
        {
            return orig_this.rotateLatLng(NodeLatitude, NodeLongitude);
        }
        else
        {
            return xy(NodeLongitude, NodeLatitude);
        }
    };

    Indoor2DMap.prototype.rotateLatLng = function(latitude, longitude)
    {
        var pointCur = new L.latLng(latitude, longitude);
        var pointOrigin = new L.latLng(this.mWGS48OriginY, this.mWGS48OriginX);
        return rotatePoint(pointCur, this.mMAP_North, pointOrigin);
    };

    //
    // initial
    //
    this.mMap.on('overlayadd', this.onOverlayAdd);
    this.mMap.on('overlayremove ', this.onOverlayRemove);
    this.create_alert_scale_icon();

    //
    // draw tag on map every 100 ms
    //
    if (this.mEnableUISmoothing)
    {
        setTimeout(this.draw_tag_smooth_move, 2000, this);
        setTimeout(this.draw_tag_GPS_smooth_move, 2000, this);
    }

    //
    // update alert circle
    //
    setTimeout(this.update_alert_circle, 2000, this);

    // function loc(latitude,longitude,c)
    // {
    // 	var lat_diff=c/110.574;  //利用距離的比例來算出緯度上的比例
    // 	var lon_distance=111.320*Math.cos(latitude*Math.PI/180); //算出該緯度上的經度長度
    // 	var lon_diff=c/lon_distance; //利用距離的比例來算出經度上的比例
    //
    // 	var N = latitude + Math.abs(lat_diff),
    // 	S = latitude - Math.abs(lat_diff),
    // 	E = longitude+ Math.abs(lon_diff),
    // 	W = longitude- Math.abs(lon_diff);
    // 	console.info(latitude,E)
    // 	console.info(S,longitude)
    // 	console.info(latitude,W)
    // 	console.info(N,longitude)
    // }

    function getCurrent_r(status)
    {
        var r = 0.1;
        switch(status)
        {
            case 1:
            case 2:
                r = 0.1;
            break;
            case 3:
            case 4:
            case 5:
            case 6:
                r = 0.8;
            break;
        }
        return r;
    };

    //
    //    Latitude: 1 deg = 110.574 km
    //    Longitude: 1 deg = 111.320*cos(latitude) km
    //
    function centimeterToLatitude(distenceincm)
    {
        var lat_diff = FloatDiv(distenceincm, 11057400);  //利用距離的比例來算出緯度上的比例

    	return lat_diff;
    }

    function centimeterToLongitude(latitude, distenceincm)
    {
        var lon_distance = 11132000 * Math.cos(latitude * Math.PI/180); //算出該緯度上的經度長度
        var lon_diff = FloatDiv(distenceincm, lon_distance); //利用距離的比例來算出經度上的比例
        return lon_diff;
    }

    function LatitudeToCentimeter(latitude)
    {
        return FloatMul(latitude, 11057400);
    }

    function LongitudeToCentimeter(latitude, longitude)
    {
        var lon_distance = 11132000 * Math.cos(latitude * Math.PI/180); //算出該緯度上的經度長度

        return FloatMul(longitude, lon_distance);
    }

    function centimeterTolatLng(distenceincm)
    {
    	return parseFloat((distenceincm / 100.0) * 0.0000089987);
    }

    function latLngTocentimeter(distenceinlatLng)
    {
    	return parseFloat((distenceinlatLng / 0.0000089987) * 100.0 );
    }

    function getRandomColor()
    {
        var letters = '0123456789ABCDEF';
        var color = '#';
        for (var i = 0; i < 6; i++ ) {
            color += letters[Math.floor(Math.random() * 16)];
        }
        return color;
    }
};
 // IndoorMap2D

    //
    // Color picker
    //
    $(document).ready( function()
    {
        $('.colorpicker').each( function()
        {
            $(this).minicolors(
            {
                changeDelay: 200,
                format: 'hex',
                position: 'bottom left',
                theme: 'bootstrap',
                opacity: $(this).attr('data-opacity'),
                change: function(fillColorhex, opacity)
                {
                    console.log("change() fillColorhex:"+fillColorhex +", opacity:"+opacity);

                    var Opacity = parseInt(opacity * 255);
                    var hex = Opacity.toString(16);
                    $('#areacolor_input').val(fillColorhex + hex);
                },
                theme: 'default'
            });
        });
    });
