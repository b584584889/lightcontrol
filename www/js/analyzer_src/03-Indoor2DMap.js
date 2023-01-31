/*
 * Author: ikki Chung
 * Date: 19 Jan 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

  // global variable
  //var usermap_width = 600;
  //var usermap_height = 600;
  //var map_scale = 56.85;
  //var map_coeff = 100000.0;
  //var map_origX = 0.0;
  //var map_origY = 0.0;
  //var map_origZ = 0.0;
  //var usr_map_url = "/build/images/2dgrid.png";
  //var project_name = "Indoor 2D Map";
  //var can_edit_map = "False";
  //var LiveMapActive = "active";

function Indoor2DMap()
{


	//
	// Coordinates in CRS.Simple take the form of [y, x] instead of [x, y]
	//
	var yx = L.latLng;

	var xy = function(x, y) {
		if (L.Util.isArray(x)) {    // When doing xy([x, y]);
			return yx(x[1], x[0]);
		}
		return yx(y, x);  // When doing xy(x, y);
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

	var anchorIcon = L.icon({
	    iconUrl: '/images/anchor-icon-2x.png',
	    shadowUrl: '/images/marker-shadow.png',

	    iconSize:     [25, 37], // size of the icon
	    shadowSize:   [41, 41], // size of the shadow
	    iconAnchor:   [14, 37], // point of the icon which will correspond to marker's location
	    shadowAnchor: [14, 41],  // the same for the shadow
	    popupAnchor:  [0, -35] // point from which the popup should open relative to the iconAnchor
	});

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
    var layer_building_nodes = [];
    var layer_trace_type = [];
    var layer_trace_all = [];
    var mBuildingID = 1;
    var mBuildingFloor = 1;

    var OriginMarker = null;
	var Coords = [];
	var Anchors = [];
	var Tags = [];
    var MapAreas = [];
    var glAllTagName = [];
    var glAllAnchorName = [];

    // map area border
    var areaborder_color = '#8b95aa';
    var areaborder_opacity = '0.8';

    var FadeOutPolylines = [[]];
    var FadeOutNodes = [[]];
    var TagPredictRanging = [];

	// v1/api
    var AnchorInfoList = [];
    var TagInfoList = [];
    var traceList = [];

    var mNodeDragCallback = null;
    var mOrigNodeDragCallback = null;
    var mUpdateMapInfoCallback = null;
    var mNodeClickCallback = null;

    //
	// Add Control
	//
    var mEdit_mode = false;
    var mEdit_delete_mode = false;
 	var mEdit_maparea_index = -1;
	var mUsermapEdit = null;
    var mUsermapScaleUp = null;
    var mUsermapScaleDown = null;
    var mLayerControl = null;

    var mOrigX = 0;
    var mOrigY = 0;
    var mUsermap_width = 0;
    var mUsermap_height = 0;
    var mMap_scale = 0;
    var mMap_coeff = 0;
    var mBounds = [xy(0, 0), xy(1000, 1000)];

    var mUserMaplayer = null;
    var mUserMap_url = null;
    var mUserProject_Name = null;

	this.baseLayers = {
		//"Diamond": Diamondlayer
	};

	// Leaflet.draw
	// Initialize the FeatureGroup to store editable layers
	var layer_areas = new L.FeatureGroup();

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
     	openPopup: function (popup, latlng, options) {
            if (!(popup instanceof L.Popup)) {
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

    var mMap = L.map('mapid', {
		crs: L.CRS.Simple,
		minZoom: 15,
		maxZoom: 26,
		zoomControl: true,
        zoomSnap: 0.25,
		zoomDelta: 0.5,
		wheelPxPerZoomLevel: 50,
		doubleClickZoom: false
		//layers: [layer_areas]
        //layers: [layer_coords, layer_anchors, layer_tags, layer_areas]
	});
	//mMap.addLayer(UserMaplayer, project_name);

	//=======
	//mMap.fitBounds(bounds, {paddingTopLeft: [0,0]});
	//=======

	//
	// control
	//
	//mMap.zoomControl.setPosition('topright');
	//var layerControl = L.control.layers(baseLayers, overlays).addTo(mMap);
	//L.control.scale({imperial: false, maxWidth: 300}).addTo(mMap);

    function update_2d_map()
    {
		mMap.removeLayer(mUserMaplayer);

        //var origX = centimeterTolatLng(map_origX * -1.0);
        //var origY = centimeterTolatLng(map_origY * -1.0);
		//var bounds = [xy(origX, origY), xy(origX + (usermap_width/(map_scale*map_coeff)), origY + (usermap_height/(map_scale*map_coeff)))];
		//UserMaplayer = L.imageOverlay(usr_map_url, bounds, {attribution: project_name, opacity: 0.5});
		//mMap.addLayer(UserMaplayer, project_name);

        //alert("mOrigX=" + mOrigX +", mOrigY="+mOrigY);
        //alert("mUsermap_width=" + mUsermap_width +", mUsermap_height="+mUsermap_height);
        //alert("mMap_scale=" + mMap_scale +", mMap_coeff="+mMap_coeff);


        var paraX = mOrigX + (mUsermap_width/(mMap_scale*mMap_coeff));
        var paraY = mOrigY + (mUsermap_height/(mMap_scale*mMap_coeff));

        //alert("paraX=" + paraX +", paraY="+paraY);

		mBounds = [xy(mOrigX, mOrigY), xy(paraX, paraY)];

        //mBounds = [xy(mOrigX, mOrigY), xy(mOrigX + (mUsermap_width/(mMap_scale*mMap_coeff)), mOrigY + (mUsermap_height/(mMap_scale*mMap_coeff)))];
		mUserMaplayer = L.imageOverlay(mUserMap_url, mBounds, {attribution: mUserProject_Name, opacity: 0.5});
		mMap.addLayer(mUserMaplayer, mUserProject_Name);

    }

    this.setMapCoeff = function(map_origX, map_origY, width, height, scale, coeff)
    {
        if (map_origX == null )
            map_origX = 0.0;
        if (map_origY == null )
            map_origY = 0.0;
        mOrigX = centimeterTolatLng(parseFloat(map_origX) * -1.0);
        mOrigY = centimeterTolatLng(parseFloat(map_origY) * -1.0);
        mUsermap_width = parseInt(width);
        mUsermap_height = parseInt(height);
        mMap_scale = parseFloat(scale);
        mMap_coeff = parseFloat(coeff);

        //alert("mMap_scale = "+ mMap_scale);

        mBounds = [xy(mOrigX, mOrigY), xy(mOrigX + (mUsermap_width/(mMap_scale*mMap_coeff)), mOrigY + (mUsermap_height/(mMap_scale*mMap_coeff)))];
    }

    this.setMapImage = function(usr_map_url, project_name)
    {
        mUserMap_url = usr_map_url;
        mUserProject_Name = project_name;
        mUserMaplayer = L.imageOverlay(usr_map_url, mBounds, {attribution: project_name, opacity: 0.5});

        mMap.addLayer(mUserMaplayer, project_name);

        //=======
    	mMap.fitBounds(mBounds, {paddingTopLeft: [0,0]});
    	//=======
        //map.setZoom(map_zoom);
    	//=======

        //
    	// control
    	//
    	mMap.zoomControl.setPosition('topright');
    	mLayerControl = L.control.layers(this.baseLayers, this.overlays).addTo(mMap);
    	L.control.scale({imperial: false, maxWidth: 300}).addTo(mMap);
    	//L.control.mousePosition().addTo(map);

        // area layer:
        // mLayerControl.addOverlay(layer_areas, "Areas");

        create_map_action(mMap);
    }

    this.setBuildingInfo = function(building_id, building_floor)
    {
        mBuildingID = building_id;
        mBuildingFloor = building_floor;
    }

	//
	// change map
	//
	this.changeUserMap = function(new_map_url, new_map_width, new_map_height)
	{
        mUserMap_url = new_map_url;
    	mMap.removeLayer(mUserMaplayer);

        mUsermap_width = new_map_width;
        mUsermap_height = new_map_height;

    	bounds = [xy(0, 0), xy(mUsermap_width/(mMap_scale*mMap_coeff), mUsermap_height/(mMap_scale*mMap_coeff))];
    	mUserMaplayer = L.imageOverlay(mUserMap_url, bounds, {attribution: mUserProject_Name, opacity: 0.5});
    	mMap.addLayer(mUserMaplayer, mUserProject_Name);

		//mUserMap_url.setUrl(mapurl);
	}

    //
    // check current map editor mode
    //
    this.editmode = function()
    {
        return mEdit_mode;
    }

    //
    //addOriginMarker(0, 0, 0);
    //

	function markerOnClick(e)
	{
	    //alert(e.title);
	    markclick(e.target.type, e.target.index);

        if (typeof mNodeClickCallback === 'function')
            mNodeClickCallback(e.target);
	}

    var mapAreaOnClick = function(e)
	{
	    if(mEdit_mode && !mEdit_delete_mode)
	    {
	       layer = MapAreas[e.target.index];
	       var shape = layer.toGeoJSON();
	       toggle_map_area_editor(shape, e.target.index);
	    }
	}

	function centimeterTolatLng(distenceincm)
	{
		return parseFloat((distenceincm / 100.0) * 0.0000089987);
	}

	function latLngTocentimeter(distenceinlatLng)
	{
		return parseFloat((distenceinlatLng / 0.0000089987) * 100.0 );
	}

	//
	// create VILS marker
	//
	function createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, draggable)
	{
        var popName = nodename;
        if ( nodename.length == 0 )
            popName = nodeid;

        //var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:false, maxWidth: "50"})
        //    .setContent("<img src=\"" + "/build/images/"+nodename+".png\"" + " width=\"40\" height=\"40\" />");

        var newMarker = L.marker(xy(centimeterTolatLng(posX), centimeterTolatLng(posY)), {title:nodeid, draggable: draggable}).bindPopup(popName);
		newMarker.nodeid = nodeid;
        newMarker.macaddress = macaddress;
		newMarker.nodename = nodename;
		newMarker.posX = posX;
		newMarker.posY = posY;
		newMarker.posZ = posZ;
		newMarker.date = date;
		newMarker.on('click', markerOnClick);

        // dragend, drag
		newMarker.on('dragend', function(event){
	    	var marker = event.target;
	    	var position = marker.getLatLng();
	    	//mMap.panTo(new L.LatLng(position.lat, position.lng));

	        marker.posX = latLngTocentimeter(position.lng).toFixed(2);
	        marker.posY = latLngTocentimeter(position.lat).toFixed(2);

            if (typeof mNodeDragCallback === "function")
            {
                //alert('typeof mNodeDragCallback === "function"');
                //alert("type=" +marker.type + ", nodeid=" + marker.nodeid+ ", posX=" + marker.posX+ ", posY=" + marker.posY+ ", posZ=" + marker.posZ);
                mNodeDragCallback(marker.type, newMarker.macaddress, marker.nodeid, marker.posX, marker.posY, marker.posZ);
            }
            else
            {
                //alert('typeof mNodeDragCallback not "function"');
            }

	        //update_node_position_onGAE(mUser_id, mProject_id, mToken, marker.nodeid, marker.posX, marker.posY, marker.posZ, 0, 0, 0, 0, 0);
	  	});


		return newMarker;
	}

    //
    // fade out circle
    //
    function fadeout_tag_footprint(tag_node_id, buildingid, floor)
    {
        var pointList = [];
        var existFadeOutNodes = [];

        var TagFadeOutline = FadeOutPolylines[tag_node_id];
        var TagFadeOutNodes = FadeOutNodes[tag_node_id];

        var current_layer = getCurrentLayer("Tags", buildingid, floor);

        if (TagFadeOutline != null)
        {
            current_layer.removeLayer(TagFadeOutline);
            //mMap.removeLayer(TagFadeOutline);
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
            //layer.options.fillOpacity = layer.options.opacity;
            //layer.options.opacity = areaborder_opacity;

            if (old_radius <= 0.5)
            {
                current_layer.removeLayer(circle);
                //TagFadeOutNodes.splice(i, 1);
                //i--;
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
        FadeOutNodes[tag_node_id] = existFadeOutNodes;
//456
        TagFadeOutline = new L.Polyline(pointList, {
        color: circleColor,
        weight: 5,
        opacity: 0.4,
        smoothFactor: 1
        });
        current_layer.addLayer(TagFadeOutline);
        //mMap.addLayer(TagFadeOutline);
        FadeOutPolylines[tag_node_id] = TagFadeOutline;

    }

    function fadeout_loop()
    {
        for	(index = 0; index < Tags.length; index++)
        {
            tag_node = Tags[index];
            fadeout_tag_footprint(tag_node.nodeid, tag_node.buildingid, tag_node.floor);
        }

        setTimeout(fadeout_loop, 1000);
    }



    this.createMapEditor = function()
    {
        mUsermapScaleDown = new UserMapScaleDownControl();
        mUsermapScaleUp = new UserMapScaleUpControl();
        //mUsermapEditBtn = new MapEditBtnControl();
        mUsermapEdit = new UserMapEditControl();
        mMap.addControl(mUsermapEdit);
        //mMap.addControl(mUsermapEditBtn);

        //create_map_action(mMap);
        //load_maparea();

        fadeout_loop();
    }

    this.setNodeDragCallback = function(callback)
    {
        mNodeDragCallback = callback;
    }

    this.setOrigNodeDragCallback = function(callback)
    {
        mOrigNodeDragCallback  = callback;
    }

    this.setUpdateMapInfoCallback = function(callback)
    {
        mUpdateMapInfoCallback = callback;
    }

    this.setNodeClickCallback = function(callback)
    {
        mNodeClickCallback = callback;
    }

    //
	// add origin marker
	//
    this.addOriginMarker = function(posX, posY, posZ)
	{
		var newMarker = createVILSMarker('0000', 'OriginXYZID', 'Origin', posX, posY, posZ, null, false);

		newMarker.setIcon(origIcon);
        //newMarker.addTo(mMap);
        var current_layer = getCurrentLayer("Origin", 1, 1);
		newMarker.addTo(current_layer);
		OriginMarker = newMarker;
		newMarker.type = 'Origin';
		newMarker.index = 0;


        // dragend, drag
        newMarker.on('dragend', function(event){
        	var marker = event.target;
        	var position = marker.getLatLng();
            var latLngX = position.lng - mOrigX;
            var latLngY = position.lat - mOrigY;

            mOrigX = latLngX * -1.0;
            mOrigY = latLngY * -1.0;

            var new_X = latLngTocentimeter(latLngX);
            var new_Y = latLngTocentimeter(latLngY);

            //
            // update imageOverlay
            //
            update_2d_map();

            // reset OriginMarker to Origin
            marker.setLatLng(new L.LatLng(0, 0));

            mMap.panTo(marker.getLatLng());

            if (typeof mOrigNodeDragCallback === 'function')
                mOrigNodeDragCallback(new_X, new_Y);
        });

	}

	//
	// add new Coordinator marker
	//
	this.addCoordinator = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
	{
		var newMarker = createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false);

        //
        // find same build and floor
        //
        var current_layer = getCurrentLayer("Coordinators", buildingid, floor);

		newMarker.setIcon(coordIcon);
		//newMarker.addTo(layer_coords);
        newMarker.addTo(current_layer);
	    Coords[index] = newMarker;
		newMarker.type = 'coord';
		newMarker.index = index;

	}

	//
	// add new Anchor marker
	//
	this.addAnchor = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
	{
		var newMarker = createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false);

        //
        // find same build and floor
        //
        var current_layer = getCurrentLayer("Anchors", buildingid, floor);

		newMarker.setIcon(anchorIcon);
		//newMarker.addTo(layer_anchors);
        newMarker.addTo(current_layer);
		Anchors[index] = newMarker;
		newMarker.type = 'anchor';
		newMarker.index = index;
	}

	//
	// add new Tag marker
	//
	this.addTag = function(index, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
	{
		var newMarker = createVILSMarker(macaddress, nodeid, nodename, posX, posY, posZ, date, false);

        // if (nodename == 'WilliamWu')
        // {
        //     var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:false, maxWidth: "90"})
        //         .setContent("<img src=\"" + "/images/"+nodename+".png\"" + " width=\"80\" height=\"80\" />");
        //
        //     newMarker.bindPopup(markerpopup);
        // }
        // else
        // {
        //     var markerpopup = new L.popup({minWidth:"16", closeButton:true, closeOnClick:false, maxWidth: "50"})
        //         .setContent("<img src=\"" + "/images/"+nodename+".png\"" + " width=\"40\" height=\"40\" />");
        //
        //     newMarker.bindPopup(markerpopup);
        // }

        //
        // find same build and floor
        //
        var current_layer = getCurrentLayer("Tags", buildingid, floor);

		newMarker.setIcon(tagIcon);
        //newMarker.addTo(current_layer).openPopup();
        current_layer.addLayer(newMarker);
        newMarker.openPopup();
        Tags[index] = newMarker;
		newMarker.type = 'tag';
		newMarker.index = index;
	}

    function getCurrentLayer(prefix, buildingid, floor)
    {
        var target_layer = prefix +"_" + buildingid + "_" + floor;
        var current_layer = layer_building_nodes[target_layer];
        if (current_layer == undefined)
        {
            //alert("layer_building_anchors["+target_layer+"] == undefined");
            current_layer = new L.LayerGroup();
            layer_building_nodes[target_layer] = current_layer;
            if ( prefix ==  "Tags" )
            {
                // show tags layer by default
                mMap.addLayer(current_layer, target_layer);
            }
            mLayerControl.addOverlay(current_layer, target_layer);
        }
        return current_layer;
    }

    function getTraceLayer(prefix, tagname, type)
    {
        var trace_layer = prefix +"_"+ tagname +"_"+ type;
        var current_layer = layer_trace_type[trace_layer];
        if (current_layer == undefined)
        {
            //console.log("getTraceLayer["+trace_layer+"] == undefined");
            current_layer = new L.LayerGroup();
            current_layer.tagname = tagname;
            current_layer.trace_layer = trace_layer;
            layer_trace_type[trace_layer] = current_layer;
            //if ( prefix ==  "Tags" )
            {
                // show tags layer by default
                mMap.addLayer(current_layer, trace_layer);
            }
            mLayerControl.addOverlay(current_layer, trace_layer);
            layer_trace_all.push(current_layer);
        }
        else
        {
            //console.log("getTraceLayer["+trace_layer+"] existed!");
        }
        return current_layer;
    }

  	this.updateCoordinatorMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
    {
        //
        // check building id and floor
        //
        if ( (mBuildingID != 99 && buildingid != 99) && (buildingid != mBuildingID || floor != mBuildingFloor))
        {
            // not in this map
            return;
        }

        var found = updateMarker(Coords, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false);
        if (!found)
        {
            this.addCoordinator(Coords.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date);
        }
    }

  	this.updateAnchorMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
    {
        //
        // check building id and floor
        //
        if ( (mBuildingID != 99 && buildingid != 99) && (buildingid != mBuildingID || floor != mBuildingFloor))
        {
            // not in this map
            return;
        }

        var found = updateMarker(Anchors, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false);
        if (!found)
        {
            this.addAnchor(Anchors.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date);
        }
    }

  	this.updateTagMarker = function(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date)
    {
        //
        // check building id and floor
        //
        if ( (mBuildingID != 99 && buildingid != 99) && (buildingid != mBuildingID || floor != mBuildingFloor))
        {
            // not in this map
            //alert("hideMarker:"+macaddress+":"+nodeid);
            setMarkerVisible(Tags, nodeid, false);
            return;
        }

        var found = updateMarker(Tags, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, true);
        if (!found)
        {
            this.addTag(Tags.length, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date);
        }
    }

  	function updateMarker(nodeList, macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, fadeoutFootprint)
    {
        var found = false;
        var exist_node = null;

        // find the exist node
        for	(index = 0; index < nodeList.length; index++)
        {
            exist_node = nodeList[index];
            if ( exist_node.nodeid == nodeid )
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
                //convert to local time
                //exist_node.date = moment.utc(node.date).local().format("YYYY-MM-DD HH:mm:ss");
                //exist_node.date = moment.utc(node.date, 'YYYY-MM-DD HH:mm:ss').local().format("YYYY-MM-DD HH:mm:ss");
                exist_node.date = date;
                //exist_node.location = location;
                exist_node.macaddress = macaddress;

                //if (itemindex == 0)
                //    $("#testMarkerUpdate").text("("+node.posX+","+node.posY+","+node.posZ+"), data.length="+data.length);
                //
                // update UI
                //
                var latLngX = centimeterTolatLng(parseFloat(posX));
                var latLngY = centimeterTolatLng(parseFloat(posY));
                if ( isNaN(latLngX) || isNaN(latLngY))
                {
                    //alert("showTagMarkerOnMap() ERROR!! footprintnode.posX="+footprintnode.posX+", posY="+footprintnode.posY +", footprintnode.date="+footprintnode.date);
                    continue;
                }

                exist_node.setLatLng(xy(latLngX, latLngY));

                //
                // create fadeout foot print
                //
                if (fadeoutFootprint)
                {
                    if (exist_node._icon != undefined && exist_node._icon.style.visibility == 'hidden')
                    {
                        showMarker(exist_node);
                    }

                    var colorIdx = index;
                    if (colorIdx >= 10)
                    {
                        colorIdx -= 10;
                    }
                    var curColor = footprintcolors[colorIdx];
                    var fillColor = footprintcolors[colorIdx];

                    var circle = L.circleMarker(xy(latLngX, latLngY), { color:curColor, opacity: 0.3, fillColor:fillColor, fillOpacity: 0.2, className: "geoData", radius: 5 });
                    var current_layer = getCurrentLayer("Tags", buildingid, floor);
                    //circle.addTo(mMap);
                    circle.buildingid = buildingid;
                    circle.floor = floor;
                    circle.addTo(current_layer);

                    var TagFadeOutNodes = FadeOutNodes[exist_node.nodeid];
                    if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
                        TagFadeOutNodes = [];
                    TagFadeOutNodes[ TagFadeOutNodes.length ] = circle;
                    FadeOutNodes[exist_node.nodeid] = TagFadeOutNodes;

                }

                found = true;
                break;
            }
        }
        return found;
    }
    //
  	// this.updateTagPredictPos = function(macaddress, predictRadius, predictPosX, predictPosY, predictPosZ)
    // {
    //     var layer_predictRanging = getCurrentLayer("Predict", mBuildingID, mBuildingFloor);
    //
    //     //
    //     // Tag Predict ranging
    //     //
    //     var circleradius = parseFloat(predictRadius) / 100.0;
    //     //var circleradius = 0.5; // in meters
    //     var latLngX = centimeterTolatLng(parseFloat(predictPosX));
    //     var latLngY = centimeterTolatLng(parseFloat(predictPosY));
    //     if ( isNaN(latLngX) || isNaN(latLngY))
    //     {
    //         //alert("showTagMarkerOnMap() ERROR!! footprintnode.posX="+footprintnode.posX+", posY="+footprintnode.posY +", footprintnode.date="+footprintnode.date);
    //         console.log("this.updateTagPredictPos()isNaN(latLngX) || isNaN(latLngY) ");
    //         return;
    //     }
    //
    //     // find the exist node
    //     var index;
    //     for	(index = 0; index < Tags.length; index++)
    //     {
    //         exist_node = Tags[index];
    //         if ( exist_node.macaddress == macaddress )
    //         {
    //
    //             if (TagPredictRanging[index] == null)
    //             {
    //                 var curColor = footprintcolors[index];
    //                 var fillColor = footprintcolors[index];
    //
    //                 //var newcircle = L.circleMarker(xy(latLngX, latLngY), { color:curColor, opacity: 0.6, fillColor:fillColor, fillOpacity: 0.4, className: "geoData", radius: circleradius });
    //                 var newcircle = L.circle([latLngY, latLngX], circleradius).addTo(layer_predictRanging);
    //                 newcircle.setStyle({color:curColor, opacity: 0.6, fillColor:fillColor, fillOpacity: 0.4});
    //                 newcircle.addTo(layer_predictRanging);
    //                 TagPredictRanging[index] = newcircle;
    //             }
    //             //console.log("circleradius="+circleradius+", latLngX="+latLngX+", latLngY="+latLngY);
    //
    //             TagPredictRanging[index].setStyle({radius:circleradius});
    //             TagPredictRanging[index].setLatLng(xy(latLngX, latLngY));
    //         }
    //     }
    // }

    //var type = 0; // 0: raw coordiate, 1: apply kalman coordinate
    function create_trace(traceFileName, traceCSVArray, tagname, type)
    {
        //console.log("create_trace() traceFileName:"+traceFileName +", tagname:"+tagname+", type:"+type);
        var layer_trace = getTraceLayer(traceFileName, tagname, type);

        //console.log("create_trace() layer_trace:"+layer_trace);


        var exist_trace = null;
        //var nodeid = 'tag_' + tagname +'_' + traceFileName;
        // find the exist trace
        for	(index = 0; index < traceList.length; index++)
        {
            var found = false;
            exist_trace = traceList[index];
            if ( exist_trace.tagname == tagname )
            {
                found = true;
            }
        }

        if (!found)
        {
            //console.log('updateTracker() not found exist_trace');
            exist_trace = addTrace(traceList.length, traceFileName, tagname, type);
        }

        create_trace_path_on_layer(layer_trace, exist_trace, traceFileName, type, traceCSVArray);
    }

    this.updateTracker = function(traceFileName, traceCSVArray)
    {

        //console.log('updateTracker() traceFileName:' + traceFileName);

        //
        // find all tags
        //
        var tagIdx = getDataIndexFromCSV('tag', traceCSVArray)
        var AllTagName = [];
        var index;
        for(index in traceCSVArray)
        {
            var data = traceCSVArray[index];
            if (index == 0)
                continue;

            var curTag = data[tagIdx];
            var index = AllTagName.indexOf(curTag);
            if ( index < 0)
            {
                AllTagName.push(curTag);
            }
            var allidx = glAllTagName.indexOf(curTag);
            if ( allidx < 0)
            {
                glAllTagName.push(curTag);
            }
        }

        //
        // find all anchors
        //
        var validIdx = getDataIndexFromCSV('valid', traceCSVArray);
        var data = traceCSVArray[0];
        for(index = tagIdx + 1; index<validIdx; index++ )
        {
            glAllAnchorName.push(data[index]);
        }
        console.log("updateTracker() glAllAnchorName.length:" + glAllAnchorName.length +" glAllAnchorName:"+glAllAnchorName);

        //console.log("updateTracker() AllTagName.length:" + AllTagName.length);
        var tagidx;
        for(tagidx in AllTagName)
        {
            var tagname = AllTagName[tagidx];

            //console.log('updateTracker() tagname:' + tagname);

            create_trace(traceFileName, traceCSVArray, tagname, "raw");
            create_trace(traceFileName, traceCSVArray, tagname, "filter");
            create_trace(traceFileName, traceCSVArray, tagname, "smoothing");
        }
    }

    this.clearAll = function()
    {
        glAllTagName = [];
        glAllAnchorName = [];
        var index;
        for(index=0; index<layer_trace_all.length; index++)
        {
            mMap.removeLayer(layer_trace_all[index]);
            mLayerControl.removeLayer(layer_trace_all[index]);
        }
        layer_trace_all = [];
        layer_trace_type = [];
        traceList = [];
    }

    this.getTagList = function()
    {
        return glAllTagName;
    }

    this.showTagTrack = function(tagindex, showorhide)
    {
        //console.log("showTagTrack() tagindex:"+tagindex+", showorhide:"+showorhide+", tag:"+glAllTagName[tagindex]);

        var tagname = glAllTagName[tagindex];
        var index;
        //console.log("getTraceLayerByTagName() layer_trace_all.length:"+layer_trace_all.length);
        for(index=0; index<layer_trace_all.length; index++)
        {
            //console.log(index+" layer.tagname:"+layer_trace_all[index].tagname);
            if (tagname == layer_trace_all[index].tagname)
            {
                if (showorhide)
                {
                    mMap.addLayer(layer_trace_all[index], layer_trace_all[index].trace_layer);
                    mLayerControl.addOverlay(layer_trace_all[index], layer_trace_all[index].trace_layer);
                }
                else
                {
                    mMap.removeLayer(layer_trace_all[index]);
                    mLayerControl.removeLayer(layer_trace_all[index]);
                }
            }
        }
    }

    function addTrace(index, traceFileName, tagname, type)
    {
        var layer_trace = getTraceLayer(traceFileName, tagname, type);

        //console.log('updateTracker() traceFileName:' + traceFileName);

        var exist_trace = {};
        exist_trace.tagname = tagname;
        exist_trace.traceFileName = traceFileName;
        exist_trace.type = type;
        exist_trace.FadeOutPolylines = [];
        exist_trace.FadeOutNodes = [];
        exist_trace.DiffPolylines = [];
        exist_trace.index = index;
        traceList[index] = exist_trace;

        return exist_trace;
    }

    function create_trace_path_on_layer(layer_trace, exist_trace, traceFileName, type, traceCSVArray)
    {
        var pointList = [];

        //
        // clear old tarce path
        //
        var traceFadeOutline = exist_trace.FadeOutPolylines;
        var diffPolylines = exist_trace.DiffPolylines;
        var traceFadeOutNodes = exist_trace.FadeOutNodes;

        var layer_trace = getTraceLayer(traceFileName, exist_trace.tagname, type);
        var layer_trace_diff = getTraceLayer(traceFileName, exist_trace.tagname, "diff");

        if (traceFadeOutline != null)
        {
            layer_trace.removeLayer(traceFadeOutline);
            traceFadeOutline = null;
        }
        if (traceFadeOutNodes != null)
        {
            layer_trace.removeLayer(traceFadeOutNodes);
            traceFadeOutNodes = null;
            traceFadeOutNodes = [];
        }
        if (diffPolylines != null)
        {
            for (idx in diffPolylines)
            {
                layer_trace_diff.removeLayer(diffPolylines[idx]);
            }
            diffPolylines = null;
            diffPolylines = [];
        }
        // raw
        var rXIdx = getDataIndexFromCSV('rX', traceCSVArray);
        // kalman
        var kXIdx = getDataIndexFromCSV('kX', traceCSVArray);
        // smoothing
        var sXIdx = getDataIndexFromCSV('sX', traceCSVArray);

        var tagIdx = getDataIndexFromCSV('tag', traceCSVArray);

        var validIdx = getDataIndexFromCSV('valid', traceCSVArray);
        //console.log('create_trace_path_on_layer() type:' + type +' rXIdx:' + rXIdx + ' kXIdx:' + kXIdx);

        var colorIdx = layer_trace_all.indexOf(layer_trace);
        //var colorIdx = exist_trace.index;
        if (colorIdx >= 10)
        {
            colorIdx -= 10;
        }
        var curColor = footprintcolors[colorIdx];
        var fillColor = footprintcolors[colorIdx];
        var circleColor = 'rgb(244, 131, 66)';

        var index;
        for(index in traceCSVArray)
        {
            var data = traceCSVArray[index];
            if (index == 0)
                continue;

            //console.log('data[rXIdx+0]:' + data[rXIdx+0]);
            var tagname = data[tagIdx];
            if (exist_trace.tagname != tagname)
            {
                continue;
            }
            var valid = Number(data[validIdx]);

            var posX = 0;
            var posY = 0;
            var posZ = 0;
            if (type == "raw")
            {
                // raw
                if (rXIdx >= 0)
                {
                    posX = data[rXIdx + 0];
                    posY = data[rXIdx + 1];
                    posZ = data[rXIdx + 2];
                    //console.log('X:'+ X + ' Y:' + Y + ' Z:' + Z);
                }
            }
            else
            if (type == "filter")
            {
                // kalman
                if (kXIdx >= 0)
                {
                    posX = data[kXIdx + 0];
                    posY = data[kXIdx + 1];
                    posZ = data[kXIdx + 2];
                    //console.log('X:'+ X + ' Y:' + Y + ' Z:' + Z);
                }
            }
            else
            if (type == "smoothing")
            {
                // smoothing
                if (sXIdx >= 0)
                {
                    posX = data[sXIdx + 0];
                    posY = data[sXIdx + 1];
                    posZ = data[sXIdx + 2];
                    //console.log('X:'+ X + ' Y:' + Y + ' Z:' + Z);
                }
            }
            //console.log('X:'+ X + ' Y:' + Y + ' Z:' + Z);

            //
            // update UI
            //
            var latLngX = centimeterTolatLng(parseFloat(posX));
            var latLngY = centimeterTolatLng(parseFloat(posY));
            if ( isNaN(latLngX) || isNaN(latLngY))
            {
                //alert("showTagMarkerOnMap() ERROR!! footprintnode.posX="+footprintnode.posX+", posY="+footprintnode.posY +", footprintnode.date="+footprintnode.date);
                continue;
            }

            if (valid == 3)
            {
                curColor = 'rgb(244, 131, 66)';
                fillColor = 'rgb(50, 50, 50)';
                circleOpacity = 0.9;
                circlefillOpacity = 0.9;
            }
            else
            {
                curColor = footprintcolors[colorIdx];
                fillColor = footprintcolors[colorIdx];
                circleOpacity = 0.3;
                circlefillOpacity = 0.2;
            }


            var popUpInfo = createNodePopUpRangingInfo(data, tagIdx+1, validIdx);
            //var popIpInfo = "valid:"+valid+" data:"+data;
            var circle = L.circleMarker(xy(latLngX, latLngY), { color:curColor, opacity: circleOpacity, fillColor:fillColor, fillOpacity: circlefillOpacity, className: "geoData", radius: 5 }).bindPopup(popUpInfo);

            //circle.addTo(mMap);
            //circle.buildingid = buildingid;
            //circle.floor = floor;
            circle.addTo(layer_trace);

            // var TagFadeOutNodes = FadeOutNodes[exist_node.nodeid];
            // if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
            //     TagFadeOutNodes = [];
            traceFadeOutNodes[ traceFadeOutNodes.length ] = circle;

            circleColor = circle.options.color;
            var position = circle.getLatLng();
            pointList.push([position.lat, position.lng]);


            var diffpointList = [];

            var latLngRawX = centimeterTolatLng(parseFloat(data[rXIdx + 0]));
            var latLngRawY = centimeterTolatLng(parseFloat(data[rXIdx + 1]));
            var latLngKX = centimeterTolatLng(parseFloat(data[kXIdx + 0]));
            var latLngKY = centimeterTolatLng(parseFloat(data[kXIdx + 1]));
            //var latLngSX = centimeterTolatLng(parseFloat(data[sXIdx + 0]));
            //var latLngSY = centimeterTolatLng(parseFloat(data[sXIdx + 1]));
            diffpointList.push([latLngRawY, latLngRawX]);
            diffpointList.push([latLngKY, latLngKX]);
            //diffpointList.push([latLngSY, latLngSX]);

            diffOutline = new L.Polyline(diffpointList, {
            color: 'rgb(244, 131, 66)',
            weight: 2,
            opacity: 0.9,
            smoothFactor: 1
            });
            diffPolylines.push(diffOutline);
            layer_trace_diff.addLayer(diffOutline);
        }

        traceFadeOutline = new L.Polyline(pointList, {
        color: circleColor,
        weight: 5,
        opacity: 0.4,
        smoothFactor: 1
        });
        layer_trace.addLayer(traceFadeOutline);

        exist_trace.FadeOutPolylines = traceFadeOutline;
        exist_trace.DiffPolylines = diffPolylines;
        exist_trace.FadeOutNodes = traceFadeOutNodes;
    }

    function createNodePopUpRangingInfo(data, startIdx, endIdx)
    {
        var popupString = "";
        var index;
        for(index=0; (startIdx+index) < endIdx; index++)
        {
            var ranging = data[startIdx + index];
            var anchor = glAllAnchorName[index];
            if (ranging.length > 0)
            {
                popupString += anchor+" "+ranging+"<br>";
            }
        }
        return popupString;
    }

    function getDataIndexFromCSV(patent, traceCSVArray)
    {
        var index = -1;
        var headers = traceCSVArray[0];
        var idx = 0;
        for( idx in headers)
        {
            if (headers[idx] == patent)
            {
                index = Number(idx);
                break;
            }
        }
        //console.log('getDataIndexFromCSV() patent:' + patent + ' index:' + index);

        return index;
    }

    //
    // hide marker
    //
  	function setMarkerVisible(nodeList, nodeid, visible)
    {
        var found = false;
        var exist_node = null;

        // find the exist node
        for	(index = 0; index < nodeList.length; index++)
        {
            exist_node = nodeList[index];
            if ( exist_node.nodeid == nodeid )
            {
                if (visible)
                {
                    showMarker(exist_node);
                }
                else
                {
                    hideMarker(exist_node);
                }
                break;
            }
        }
    }

    function hideMarker(node)
    {
        //exist_node._icon.style.display = 'none';
        //exist_node._shadow.style.display = 'none';
        node._icon.style.visibility = 'hidden';
        node._shadow.style.visibility = 'hidden';
        node.closePopup();
    }

    function showMarker(node)
    {
        //exist_node._icon.style.display = 'true';
        //exist_node._shadow.style.display = 'true';
        node._icon.style.visibility = 'visible';
        node._shadow.style.visibility = 'visible';
        node.openPopup();
    }



	//
	// click handle
	//
	function markclick(maketype, markindex)
	{
		//alert("maketype = " + maketype + ", markindex=" + markindex);

		var valueIdx = markindex+1;
		if (maketype === 'coord')
		{
			Coords[markindex].openPopup();
			$("#ProfileMarkerName").text(Coords[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+Coords[markindex].posX+","+Coords[markindex].posY+","+Coords[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(Coords[markindex].date).local().format("MMM Do, hh:mm A"));
		}
		else
		if (maketype === 'anchor')
		{
			Anchors[markindex].openPopup();
			$("#ProfileMarkerName").text(Anchors[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+Anchors[markindex].posX+","+Anchors[markindex].posY+","+Anchors[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(Anchors[markindex].date).local().format("MMM Do, hh:mm A"));
 		}
		else
		if (maketype === 'tag')
		{
			Tags[markindex].openPopup();
			$("#ProfileMarkerName").text(Tags[markindex].nodename);
			$("#ProfileMarkerPosition").text("("+Tags[markindex].posX+","+Tags[markindex].posY+","+Tags[markindex].posZ+")");
			$("#ProfileMarkerUpdateTime").text(moment.utc(Tags[markindex].date).local().format("MMM Do, hh:mm A"));
 		}
        else
		if (maketype === 'Origin')
		{
            if (OriginMarker != null)
            {
                OriginMarker.openPopup();
			    $("#ProfileMarkerName").text(OriginMarker.nodename);
			    $("#ProfileMarkerPosition").text("("+OriginMarker.posX+","+OriginMarker.posY+","+OriginMarker.posZ+")");
			    $("#ProfileMarkerLocation").text("");
                $("#ProfileMarkerUpdateTime").text("");
            }
 		}
	}


    var UserMapEditControl = L.Control.extend({
        options: {
            position: 'topleft'
        },

        onAdd: function (cur_map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/edit-icon.png)";
            container.style.backgroundSize = "25px 25px";
            container.style.width = '25px';
            container.style.height = '25px';

            container.onclick = function()
            {
                if (mEdit_mode)
                {
                    cur_map.removeControl(mUsermapScaleUp);
                    cur_map.removeControl(mUsermapScaleDown);
                    cur_map.removeControl(drawControl);
                    //cur_map.removeControl(mUsermapEditBtn);
                    mEdit_mode = false;
                    $('#areaname_edit').css('display','none');

                    setMakerDragable(mEdit_mode);
                }
                else
                {
                    cur_map.addControl(mUsermapScaleUp);
                    cur_map.addControl(mUsermapScaleDown);
                    cur_map.addControl(drawControl);
                    //cur_map.addControl(mUsermapEditBtn);
                    mEdit_mode = true;

                    setMakerDragable(mEdit_mode);
                }
            }
            return container;
        }
    });

    var UserMapScaleUpControl = L.Control.extend({
        options: {
            position: 'topleft'
        },

        onAdd: function (map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/maximize-25.png)";
            container.style.backgroundSize = "25px 25px";
            container.style.width = '25px';
            container.style.height = '25px';

            container.onclick = function()
            {
                mMap_scale -= mMap_scale * 0.01;

                update_2d_map();

                if (typeof mUpdateMapInfoCallback === 'function')
                    mUpdateMapInfoCallback(mMap_scale, mMap_coeff);
            }
            return container;
        }
    });

    var UserMapScaleDownControl = L.Control.extend({
        options: {
            position: 'topleft'
        },

        onAdd: function (map) {
            // create the control container with a particular class name
            var container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');

            container.style.backgroundColor = 'white';
            container.style.backgroundImage = "url(/images/minimize-25.png)";
            container.style.backgroundSize = "25px 25px";
            container.style.width = '25px';
            container.style.height = '25px';

            container.onclick = function()
            {
                mMap_scale += mMap_scale * 0.01;

                update_2d_map();

                if (typeof mUpdateMapInfoCallback === 'function')
                    mUpdateMapInfoCallback(mMap_scale, mMap_coeff);
            }
            return container;
        }
    });

    // Initialize the draw control and pass it the FeatureGroup of editable layers
	var drawControl = new L.Control.Draw({
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
 	        marker: false // Turns off this drawing tool
    	},
	    edit: {
	        featureGroup: layer_areas
	    }
	});

    var setMakerDragable = function(dragable)
    {
        var layer_origin = getCurrentLayer("Origin", mBuildingID, mBuildingFloor);
        if ( mMap.hasLayer(layer_origin) )
        {
            if ( OriginMarker != null && OriginMarker.dragging != null)
            {
                if (dragable)
                    OriginMarker.dragging.enable();
                else
                    OriginMarker.dragging.disable();
            }
        }

        var layer_coord = getCurrentLayer("Coordinators", mBuildingID, mBuildingFloor);
        if ( mMap.hasLayer(layer_coord) )
        {
            for	(index = 0; index < Coords.length; index++)
            {
                coord_node = Coords[index];
                if ( coord_node == null || coord_node.dragging == null)
                    continue;
                if (dragable)
                    coord_node.dragging.enable();
                else
                    coord_node.dragging.disable();
            }
        }

        var layer_tags = getCurrentLayer("Tags", mBuildingID, mBuildingFloor);
        if ( mMap.hasLayer(layer_tags) )
        {
            for	(index = 0; index < Tags.length; index++)
            {
                tag_node = Tags[index];
                if ( tag_node == null || tag_node.dragging == null)
                    continue;
                if (dragable)
                    tag_node.dragging.enable();
                else
                    tag_node.dragging.disable();
            }
        }


        var layer_anchors = getCurrentLayer("Anchors", mBuildingID, mBuildingFloor);
        if ( mMap.hasLayer(layer_anchors) )
        {
            for	(index = 0; index < Anchors.length; index++)
            {
                anchor_node = Anchors[index];
                if ( anchor_node == null || anchor_node.dragging == null)
                    continue;
                if (dragable)
                    anchor_node.dragging.enable();
                else
                    anchor_node.dragging.disable();
            }
        }
    }

    //
    // load GeoJSON from GAE
    //
    this.load_maparea = function()
    {
        get_maparea_From7688(function(data)
        {
            //alert("load_maparea data="+data);

            $.each(data, function(itemindex, area)
            {
                //alert("load_maparea area="+area);

                L.geoJson(JSON.parse(area), {
                    pointToLayer: function(feature, latlng)
                    {
                        return;
                    },
                    style: function (feature) {
                        return feature.properties.style;
                    },
                    onEachFeature: function (feature, layer) {

                        var showName = feature.properties.areaname;
                        layer.bindPopup(showName);
                    }
                }).eachLayer(function(layer)
                {
                    layer.index = itemindex;
                    layer.on('click', mapAreaOnClick);
                    MapAreas[itemindex] = layer;
                    layer_areas.addLayer(layer);
                });
            });
        });
    }

    var create_map_success = function(response, area_index)
    {
        var oldlayer = MapAreas[area_index];
        layer_areas.removeLayer(oldlayer);

        //alert("create_map_success() response:"+response);
        create_map_area(response['geojson'], area_index);
    }

    var create_map_action = function(map)
    {
        map.on('draw:created', function (e)
    	{
    	    var type = e.layerType,
    	        layer = e.layer;

            layer.options.fillColor = layer.options.color;
            layer.options.fillOpacity = layer.options.opacity;
            layer.options.color = areaborder_color;
            layer.options.opacity = areaborder_opacity;

            layer.index = MapAreas.length;
            layer.on('click', mapAreaOnClick);
            MapAreas[MapAreas.length] = layer;
            layer_areas.addLayer(layer);

    	    var shape = layer.toGeoJSON();
            // add style
    		//alert("draw:created layer.options:"+ JSON.stringify(layer.options));
    		shape.properties.style = {};
            shape.properties.style.color = layer.options.color;
            shape.properties.style.opacity = layer.options.opacity;
            shape.properties.style.weight = layer.options.weight;
            //shape.properties.style.fillColor = layer.options.fillColor;
            shape.properties.style.fillColor = getRandomColor();
            shape.properties.style.fillOpacity = layer.options.fillOpacity;

            // get random area id
            shape.properties.areaid = "area" + getRandomInt(1000, 9999);
            shape.properties.areaname = "Area " + MapAreas.length;

    	    var shape_for_db = JSON.stringify(shape);
    	    //alert("draw:created shape_for_db:"+shape_for_db);

    		//
    		// create new GeoJSON on GAE
    		//
    		create_map_area_on7688(shape_for_db, shape.properties.areaid, layer.index, create_map_success);
    	});

    	map.on('draw:edited', function (e)
    	{
    	    // Update db to save latest changes.
    	    var layers = e.layers;
        	layers.eachLayer(function (layer)
        	{
    	        var shape = layer.toGeoJSON();
    	        var area_id = shape.properties.areaid;
                var shape_for_db = JSON.stringify(shape);
                //alert("draw:edited shape_for_db:"+shape_for_db);

     	    	//alert("draw:edited layer = " + layer.constructor.name +", layer.getLatLngs() = " + layer.getLatLngs());

       			//
       			// update GeoJSON
       			//
                update_map_area_on7688(area_id, shape_for_db, shape.properties.areaname);
                //alert("update_map_area area_id:"+area_id+",shape_for_db:"+shape_for_db);
        	});

    	});

    	map.on('draw:deletestart', function (e)
    	{
    	   mEdit_delete_mode = true;
    	});

    	map.on('draw:deletestop', function (e)
    	{
    	   mEdit_delete_mode = false;
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
    	        delete_map_area(area_id);
        	});
            mEdit_delete_mode = false;
    	});

    	map.on('moveend', function ()
    	{
    	     //var map_center = L.latLng(0.00032, 0.00049);
    	     //map.setView(map_center);
    	});
    }

	//
	// delete a map area
	//
	var delete_map_area = function(area_id)
	{
        //alert("delete_map_area area_id:"+area_id);
        delete_map_area_on7688(area_id);
    }

    //
    // create map area from GeoJSON
    //
	var create_map_area = function(geojson, area_index)
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
                var showName = feature.properties.areaname;
	      	    //alert("create_map_area:showName = "+showName);
                layer.bindPopup(showName);
            }
        }).eachLayer(function(layer)
        {
            layer.index = area_index;
            layer.on('click', mapAreaOnClick);
            MapAreas[area_index] = layer;
            layer_areas.addLayer(layer);
        });
	}

	//
	// Color picker
	//
    $(document).ready( function() {

        $('.colorpicker').each( function() {
            $(this).minicolors({
                opacity: $(this).attr('data-opacity'),
                change: function(hex, opacity) {
                    /*
                    var log;
                    try {
                        log = hex;
                        if( opacity )
                            log += ', ' + opacity;

                        $("#testMarkerUpdate").text(log);

                    } catch(e) {}
                    */
                },
                theme: 'default'
            });

        });

    });


    function toggle_map_area_editor(shape, itemindex)
    {
        edit_maparea_index = itemindex;
        $('#areaname_input').val(shape.properties.areaname);

        $('#areaname_color').minicolors('value', shape.properties.style.fillColor);
        $('#areaname_color').minicolors('opacity', shape.properties.style.fillOpacity);

        $('#areaname_edit').css('display','block');
     }


     this.areaname_save = function()
     {
         var hex = $('#areaname_color').minicolors('value');
         var opacity = $('#areaname_color').minicolors('opacity');
         var areaname = $('#areaname_input').val();
         //var log = hex +' '+opacity;
         //$("#testMarkerUpdate").text(log);

         var layer = MapAreas[edit_maparea_index];
         layer.setStyle({color:areaborder_color, opacity:areaborder_opacity, fillColor:hex, fillOpacity:opacity});
         layer.bindPopup(areaname);

         $('#areaname_edit').css('display','none');

 //        layer.options.color = areaborder_color;
 //        layer.options.opacity = areaborder_opacity;
 //        layer.options.fillColor = hex;
 //        layer.options.fillOpacity = opacity;

         var shape = layer.toGeoJSON();

         shape.properties.areaname = areaname;
         shape.properties.style.color = areaborder_color;
         shape.properties.style.opacity = areaborder_opacity;
         shape.properties.style.fillColor = hex;
         shape.properties.style.fillOpacity = opacity;

         var area_id = shape.properties.areaid;
         var shape_for_db = JSON.stringify(shape);

         //alert("draw:edited shape_for_db:"+shape_for_db);

 		//
 		// update GeoJSON
 		//
         update_map_area_on7688(area_id, shape_for_db, areaname);
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

}
