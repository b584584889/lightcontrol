
    var camera, scene, renderer;

    var materialUV;
    var textureUV;

    var projectMapUV;
    var projectMapTextureUV;
    var GroudPlane;

    var ballUV;
    var balltextureUV;

    //var textureVILS;
    var objectSphere;
    var AnchorList = []; // UI object
    var AnchorInfoList = [];
    var AnchorLineList = [];
    var AnchorRotationList = [];
    var LocatorList = []; // UI object
    var LocatorInfoList = [];
    var LocatorLineList = [];
    var LocatorRotationList = [];
    var CoordinatorList = []; // UI object
    var CoordinatorInfoList = [];
    var CoordinatorLineList = [];
    var CoordinatorRotationList = [];
    var AoAList = []; // UI object
    var AoABackList = []; // UI object
    var AoAInfoList = [];
    var AoALineList = [];
    var AoARotationList = [];
    var AoANodeColorList = [];

    var DrawMap = true;
    var DrawTags= true;
    var DrawAnchors = false;
    var DrawLocators = false;
    var DrawCoordinators = false;
    var DrawAoAs = false;
    var DrawAoARangingCloudPoints = false;
    var DrawAoAAngleVectorCloudPoints = false;
    var DrawAoAAngleCloudPoints = false;
    var glTestAoAPowerTag = false;

    var TagList = []; // UI object
    var TagInfoList = [];
    var TagLineList = [];
    var TagRotationList = [];

    var AoARangingCloudPointList = [];
    var AoAAngleVectorCloudPointList = [];
    var AoAAngleCloudPointList = [];

    var FadeOutPolylines = [[]];
    var FadeOutNodes = [[]];
    var FadeOutNodeColors = [[]];

    var gl_anchor_num = 0;
    var gl_tag_num = 0;
    var gl_coord_num = 0;
    var gl_locator_num = 0;
    var gl_aoa_num = 0;

    var gl_anchor_load_index = 0;
    var gl_tag_load_index = 0;
    var gl_coord_load_index = 0;
    var gl_locator_load_index = 0;
    var gl_aoa_load_index = 0;

    var LOAD_DEVICE_NUM = 20;


    //var ColorsTable = [0xF48342, 0xEE82EE, 0x32CD32, 0xFF1493, 0xDAA520, 0x66CDAA, 0x00BFFF, 0xD2691E, 0x8FBC8F, 0xDDA0DD];
    var ColorsTable = [
       0xFF6347, // Tomato
       0xFFA500, // Orange
       0xFFD700, // Gold
       0x32CD32, // LimeGreen
       0x87CEEB, // SkyBlue
       0x7B68EE, // MediumSlateBlue
       0x8A2BE2, // BlueViolet
       0x4245f4, // Blue
       0xF08080, // LightCoral
       0xf441d9,  // pink
       0x000000  // black
    ];

    // for interactive
    var raycaster;
    var mouse = new THREE.Vector2(), INTERSECTED;
    var glSelectedNode = null;

    var content = document.getElementById( 'content' );

    var MAX_AXIS = 10000;
    var MARGIN = 0;
    //var ShowAspect = 0.8;
    var camera_move_delta = 20;
    var camera_lookat_point =  new THREE.Vector3(0, 0, 0);

    init();
    animate();

    function init()
    {
        scene = new THREE.Scene();

        var loShowAspect = $("#content").height() / $("#content").width() ;


        //
        // Camera
        //
		camera = new THREE.PerspectiveCamera( 70, 1/loShowAspect, 1, 10000 );
        camera.position.set(MAX_AXIS/4, -MAX_AXIS/4, MAX_AXIS/2);
        camera.up = new THREE.Vector3(0, 0, 1);
        scene.add( camera );

        //
        // Texture
        //
		var textureLoader = new THREE.TextureLoader();
        textureUV = textureLoader.load( "/images/UV_Grid_Sm.jpg" );
        textureUV.wrapS = textureUV.wrapT = THREE.RepeatWrapping;
		textureUV.anisotropy = 16;
		materialUV = new THREE.MeshBasicMaterial( { map: textureUV } );
        //========


        //========
        // football
        balltextureUV = textureLoader.load( "/images/foolball.jpg" );
        balltextureUV.wrapS = balltextureUV.wrapT = THREE.RepeatWrapping;
		balltextureUV.anisotropy = 16;
        ballUV = new THREE.MeshBasicMaterial( { map: balltextureUV } );
        //========

        //
        // sphere
        //
        //objectSphere = new THREE.Mesh( new THREE.SphereGeometry( 20, 10, 10 ), ballUV );
        objectSphere = new THREE.Mesh( new THREE.SphereGeometry( 60, 20, 20 ), materialUV );
		objectSphere.position.set( 0, 0, 0 );
		objectSphere.visible = false;
		scene.add( objectSphere );

        //
        // 3 Axis
        //
		var axis = new THREE.AxesHelper(MAX_AXIS/10);
		axis.position.set( 0, 0, 0);
		scene.add( axis );

        //
        // For interactive
        //
        raycaster = new THREE.Raycaster();


        // manager
        // var object;
		// function loadModel()
        // {
		// 	object.traverse( function ( child )
        //     {
		// 		if ( child.isMesh )
        //             child.material.map = texture;
		// 	});
        //
		// 	object.position.y = - 95;
		// 	scene.add( object );
		// }
        //
		// var manager = new THREE.LoadingManager( loadModel );
        //
		// manager.onProgress = function ( item, loaded, total )
        // {
		// 	console.log( item, loaded, total );
		// };
        //
        // // model
        // function onProgress( xhr )
        // {
		// 	if ( xhr.lengthComputable )
        //     {
		// 		var percentComplete = xhr.loaded / xhr.total * 100;
		// 		console.log( 'model ' + Math.round( percentComplete, 2 ) + '% downloaded' );
		// 	}
		// }
        //
        // function onError()
        // {
        //
        // }

        // var mtlLoader = new THREE.MTLLoader();
        // mtlLoader.setPath('js/three_src/');
        // mtlLoader.load('191217_2014.mtl', function (material)
        // {
    		// var loader = new THREE.OBJLoader( manager );
            // // loader.setMaterials(material);
    		// //loader.load( 'js/three_src/191217_2014.obj', function ( obj )
            // loader.load( 'js/three_src/male02.obj', function ( obj )
            // {
    		// 	object = obj;
    		// }, onProgress, onError );
        //});

        //
        // Render
        //
		renderer = new THREE.WebGLRenderer();
		renderer.setClearColor( 0xf0f0f0 );
		renderer.setPixelRatio( window.devicePixelRatio );
        renderer.setSize( $("#content").width(), $("#content").height());
        content.appendChild( renderer.domElement );

		window.addEventListener( 'resize', onWindowResize, false );
        content.addEventListener( 'mousemove', onDocumentMouseMove, false );

		// Controls
		controls = new THREE.OrbitControls( camera, renderer.domElement );
		controls.damping = 0.2;
		controls.addEventListener( 'change', render );


        //=========================================
        // cookies
        check_cookies();
        //=========================================
        //
        // update map
        //
        fetchMapGround();

        // update tag location
        fetchTagsnewloc();

        // creat anchors
        fetchAnchors();
        fetchCoordinators();
        fetchLocators();
        fetchAoAs();

        fadeout_loop();

        fetchAoARangingCloudPoint();
        fetchAoAAngleVectorCloudPoint();
        fetchAoAAngleCloudPoint();
        //=========================================
        //create_test_nodes();
        // fetchAoARangingCloudPoint();
        // fetchAoAAngleCloudPoint();
        //=========================================

        update_AoAPowerTagInfo(false);
	}

    function check_cookies()
    {
        DrawMap = parse_boolean(getCookie("DrawMap"), DrawMap);
        DrawTags = parse_boolean(getCookie("DrawTags"), DrawTags);
        DrawAnchors = parse_boolean(getCookie("DrawAnchors"), DrawAnchors);
        DrawLocators = parse_boolean(getCookie("DrawLocators"), DrawLocators);
        DrawCoordinators = parse_boolean(getCookie("DrawCoordinators"), DrawCoordinators);
        DrawAoAs = parse_boolean(getCookie("DrawAoAs"), DrawAoAs);
        DrawAoARangingCloudPoints = parse_boolean(getCookie("DrawAoARangingCloudPoints"), DrawAoARangingCloudPoints);
        DrawAoAAngleVectorCloudPoints = parse_boolean(getCookie("DrawAoAAngleVectorCloudPoints"), DrawAoAAngleVectorCloudPoints);
        DrawAoAAngleCloudPoints = parse_boolean(getCookie("DrawAoAAngleCloudPoints"), DrawAoAAngleCloudPoints);
        glTestAoAPowerTag = parse_boolean(getCookie("enableTestAoAPowerTag"), glTestAoAPowerTag);

        // console.log("DrawMap:"+DrawMap);
        // console.log("DrawTags:"+DrawTags);
        // console.log("DrawAnchors:"+DrawAnchors);
        // console.log("DrawLocators:"+DrawLocators);
        // console.log("DrawCoordinators:"+DrawCoordinators);
        // console.log("DrawAoAs:"+DrawAoAs);
        // console.log("DrawAoARangingCloudPoints:"+DrawAoARangingCloudPoints);
        // console.log("DrawAoAAngleVectorCloudPoints:"+DrawAoAAngleVectorCloudPoints);
        // console.log("DrawAoAAngleCloudPoints:"+DrawAoAAngleCloudPoints);
        // console.log("glTestAoAPowerTag:"+glTestAoAPowerTag);

        document.getElementById('showanchors').checked = DrawAnchors;
        document.getElementById('showcoordinators').checked = DrawCoordinators;
        document.getElementById('showlocators').checked = DrawLocators;
        document.getElementById('showtags').checked = DrawTags;
        document.getElementById('showaoas').checked = DrawAoAs;
        document.getElementById('showmap').checked = DrawMap;
        document.getElementById('showAoARangingCloudPoints').checked = DrawAoARangingCloudPoints;
        document.getElementById('showAoAAngleVectorCloudPoints').checked = DrawAoAAngleVectorCloudPoints;
        document.getElementById('showAoAAngleCloudPoints').checked = DrawAoAAngleCloudPoints;
        document.getElementById('testAoAPowerTag').checked = glTestAoAPowerTag;

        if(glTestAoAPowerTag)
            document.getElementById("testAoAPowerTag_ul").style.display = 'block';
        else
            document.getElementById("testAoAPowerTag_ul").style.display = 'none';

        tmp = getCookie("testAoAPowerTag_1");
        if (tmp != null)
            document.getElementById("testAoAPowerTag_1_InputText").value = tmp;
        tmp = getCookie("testAoAPowerTag_2");
        if (tmp != null)
            document.getElementById("testAoAPowerTag_2_InputText").value = tmp;
        tmp = getCookie("testAoAPowerTag_3");
        if (tmp != null)
            document.getElementById("testAoAPowerTag_3_InputText").value = tmp;
        tmp = getCookie("testAoAPowerTag_aoaid");
        if (tmp != null)
            document.getElementById("testAoAPowerTag_aoaid_InputText").value = tmp;
    }

    function parse_boolean(value, default_value)
    {
        if (value == null)
            return default_value;
        else
        if (value == undefined)
            return default_value;
        else
        if (value === '')
            return default_value;
        else
        if (value === 'false')
            return false;
        else
        if (value === 'true')
            return true;
    }

    function update_node_to_ui(NodeType, NodeInfoList, NodeUIList, NodeLineList, nodeData)
    {
        if (nodeData == null)
            return null;

        if (nodeData == undefined)
            return null;

        var found = false;
        var exist_node = null;
        var nodeUI = null;
        var indexUI = 0;
        // find the exist node
        for	(index = 0; index < NodeInfoList.length; index++)
        {
            exist_node = NodeInfoList[index];

            if ( exist_node.nodeid == nodeData[3] )
            {
                //var nodeinfo = {}; //replace with new data
                var nodeinfo = NodeInfoList[index];// update existed data
                nodeinfo['nodeid'] = nodeData[3];
                nodeinfo['nodename'] = decodeURIComponent(nodeData[5]);
                nodeinfo['posX'] = nodeData[7];
                nodeinfo['posY'] = nodeData[8];
                nodeinfo['posZ'] = nodeData[9];
                nodeinfo['nodedata'] = nodeData;

                NodeInfoList[index] = nodeinfo;
                nodeUI = NodeUIList[index];
                indexUI = index;
                found = true;
                break;
            }

        }
        if (!found)
        {
            // found a new node
            if (NodeType == 'Anchor')
            {
                var anchor_image = "/images/anchor-icon-2x.png";
                var textureLoader = new THREE.TextureLoader();
                var anchorTextureUV = textureLoader.load(anchor_image);
                anchorTextureUV.wrapS = anchorTextureUV.wrapT = THREE.RepeatWrapping;
                anchorTextureUV.anisotropy = 16;
                var anchorImageUV = new THREE.MeshBasicMaterial( { map: anchorTextureUV, transparent: true, side: THREE.DoubleSide } );
                nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 296), anchorImageUV);
                nodeUI.rotation.x = Math.PI / 2;

                scene.add( nodeUI );
            }
            else
            if (NodeType == 'Coordinator')
            {
                var anchor_image = "/images/coord-icon-2x.png";
                var textureLoader = new THREE.TextureLoader();
                var anchorTextureUV = textureLoader.load(anchor_image);
                anchorTextureUV.wrapS = anchorTextureUV.wrapT = THREE.RepeatWrapping;
                anchorTextureUV.anisotropy = 16;
                var anchorImageUV = new THREE.MeshBasicMaterial( { map: anchorTextureUV, transparent: true, side: THREE.DoubleSide } );
                nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 296), anchorImageUV);
                nodeUI.rotation.x = Math.PI / 2;

                scene.add( nodeUI );
            }
            else
            if (NodeType == 'Locator')
            {
                var anchor_image = "/images/locator-icon-2x.png";
                var textureLoader = new THREE.TextureLoader();
                var anchorTextureUV = textureLoader.load(anchor_image);
                anchorTextureUV.wrapS = anchorTextureUV.wrapT = THREE.RepeatWrapping;
                anchorTextureUV.anisotropy = 16;
                var anchorImageUV = new THREE.MeshBasicMaterial( { map: anchorTextureUV, transparent: true, side: THREE.DoubleSide } );
                nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 296), anchorImageUV);
                nodeUI.rotation.x = Math.PI / 2;

                scene.add( nodeUI );
            }
            else
            {
                //var geometry = new THREE.BoxGeometry( 20, 20, 20 );
                //nodeUI = new THREE.Mesh( geometry, materialUV );
                //scene.add( nodeUI );

                //================================
                // current user project  map
                //var tag_image = "/images/"+nodeData[5]+".png";
                var tag_image = "/images/marker-icon-2x.png";
                var textureLoader = new THREE.TextureLoader();
                var tagTextureUV = textureLoader.load(tag_image);
                tagTextureUV.wrapS = tagTextureUV.wrapT = THREE.RepeatWrapping;
                tagTextureUV.anisotropy = 16;
                var tagImageUV = new THREE.MeshBasicMaterial( { map: tagTextureUV, transparent: true, side: THREE.DoubleSide } );
                //================================

                //================================
                // plane
                if (nodeData[5] == 'WilliamWu')
                {
                    nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(300, 300), tagImageUV);
                }
                else
                {
                    nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 200), tagImageUV);
                }
                nodeUI.rotation.x = Math.PI / 2;
                //================================
                //  Sphere
                //nodeUI = new THREE.Mesh( new THREE.SphereGeometry( 40, 10, 10 ), ballUV );
                //================================
                scene.add( nodeUI );

                //
                // create fadeout foot print
                //
                add_new_footprint(nodeData[3], nodeData[7], nodeData[8], nodeData[9]);
            }

            var nodeinfo = {};

            nodeUI.position.x = nodeData[7];
            nodeUI.position.y = nodeData[8];
            nodeUI.position.z = nodeData[9];

            nodeinfo['nodeid'] = nodeData[3];
            nodeinfo['nodename'] = decodeURIComponent(nodeData[5]);
            nodeinfo['posX'] = nodeData[7];
            nodeinfo['posY'] = nodeData[8];
            nodeinfo['posZ'] = nodeData[9];
            nodeinfo['nodetype'] = NodeType;
            nodeinfo['nodedata'] = nodeData;

            //
            // add to list
            //
            NodeUIList.push(nodeUI);
            NodeInfoList.push(nodeinfo);

            //
            // line
            //
            var geometryline = new THREE.Geometry();
            geometryline.vertices.push( new THREE.Vector3( nodeinfo['posX'], nodeinfo['posY'], nodeinfo['posZ']) );
            geometryline.vertices.push( new THREE.Vector3( nodeinfo['posX'], nodeinfo['posY'], 0));
            //geometryline.vertices.push( new THREE.Vector3( node.posX, node.posZ, node.posY ) );
            //geometryline.vertices.push( new THREE.Vector3( node.posX, 0, node.posY ));
            var lineUI = new THREE.Line( geometryline, new THREE.LineBasicMaterial( { color: 0x8855ff, opacity: 0.6 } ) );
            NodeLineList.push(lineUI);

            scene.add( lineUI );
        }
        else
        {
            exist_node = NodeInfoList[index];

            // update posiotion
            if (nodeUI != null)
            {
                nodeUI.position.x = exist_node['posX'];
                nodeUI.position.y = exist_node['posY'];
                nodeUI.position.z = parseInt(exist_node['posZ']) + 40.0;


                //line
                var lineUI = NodeLineList[indexUI];
                lineUI.geometry.vertices[0].x = exist_node['posX'];
                lineUI.geometry.vertices[0].y = exist_node['posY'];
                lineUI.geometry.vertices[0].z = exist_node['posZ'];
                lineUI.geometry.vertices[1].x = exist_node['posX'];
                lineUI.geometry.vertices[1].y = exist_node['posY'];
                lineUI.geometry.vertices[1].z = 0.0;

                lineUI.geometry.verticesNeedUpdate = true;

                if (glSelectedNode != null)
                {
                    if (glSelectedNode.nodeid == exist_node['nodeid'])
                    {
                        document.getElementById("nodename").innerHTML = exist_node['nodename'];
                        document.getElementById("nodepos").innerHTML =
                        "("+exist_node['posX']+", " + exist_node['posY']+", " + exist_node['posZ']+")";
                    }
                }


                //
                // create fadeout foot print
                //
                if (NodeType == 'Tag')
                {
                    add_new_footprint(exist_node['nodeid'], exist_node['posX'], exist_node['posY'], exist_node['posZ']);
                }
            }
        }
    }

    function update_aoa_node_to_ui(nodeData)
    {
        if (nodeData == null)
            return null;

        if (nodeData == undefined)
            return null;

        var found = false;
        var exist_node = null;
        var nodeUI = null;
        var nodeBackUI = null;

        var indexUI = 0;
        // find the exist node
        for	(index = 0; index < AoAInfoList.length; index++)
        {
            exist_node = AoAInfoList[index];

            if ( exist_node.nodeid == nodeData[0] )
            {
                //var nodeinfo = {}; //replace with new data
                var nodeinfo = AoAInfoList[index];// update existed data
                nodeinfo['nodeid'] = nodeData[0];
                nodeinfo['nodename'] = decodeURIComponent(nodeData[1]);
                nodeinfo['posX'] = nodeData[5];
                nodeinfo['posY'] = nodeData[6];
                nodeinfo['posZ'] = nodeData[7];
                nodeinfo['nodedata'] = nodeData;

                AoAInfoList[index] = nodeinfo;
                nodeUI = AoAList[index];
                nodeBackUI = AoABackList[index];

                indexUI = index;
                found = true;
                break;
            }
        }

        if (found)
        {
            var pitch = parseInt(nodeData[8]);
            var roll  = parseInt(nodeData[9]);
            var yaw  = parseInt(nodeData[10]);

            exist_node = AoAInfoList[indexUI];

            // update posiotion
            if (nodeUI != null)
            {
                nodeData_pre = exist_node['nodedata'];

                var new_pitch = (pitch + 90) * 0.017453;
                var new_roll = roll * 0.017453;
                var new_yaw = (yaw - 90) * 0.017453;

                var show_pitch = new_pitch - nodeUI.pitch;
                var show_roll  = new_roll - nodeUI.roll;
                var show_yaw  = new_yaw - nodeUI.yaw;

                // console.log("pitch:"+pitch+" nodeUI.pitch:"+nodeUI.pitch+" show_pitch:"+show_pitch+" show_roll:"+show_roll+" show_yaw:"+show_yaw);

                var xAxis = new THREE.Vector3(1, 0, 0);
                rotateAroundWorldAxis(nodeUI, xAxis, show_pitch);// pitch

                var yAxis = new THREE.Vector3(0, 1, 0);
                rotateAroundWorldAxis(nodeUI, yAxis, show_roll);// roll
                //
                var zAxis = new THREE.Vector3(0, 0, 1);
                rotateAroundWorldAxis(nodeUI, zAxis, show_yaw);// yaw

                nodeUI.pitch = new_pitch;
                nodeUI.roll = new_roll;
                nodeUI.yaw = new_yaw;

                nodeUI.position.x = parseInt(exist_node['posX']);
                nodeUI.position.y = parseInt(exist_node['posY']);
                nodeUI.position.z = parseInt(exist_node['posZ']);

                //line
                var lineUI = AoALineList[indexUI];
                lineUI.geometry.vertices[0].x = exist_node['posX'];
                lineUI.geometry.vertices[0].y = exist_node['posY'];
                lineUI.geometry.vertices[0].z = exist_node['posZ'];
                lineUI.geometry.vertices[1].x = exist_node['posX'];
                lineUI.geometry.vertices[1].y = exist_node['posY'];
                lineUI.geometry.vertices[1].z = 0.0;

                lineUI.geometry.verticesNeedUpdate = true;
            }
            if (nodeBackUI != null)
            {
                nodeData = exist_node['nodedata'];

                var new_pitch = (pitch + 90) * 0.017453;
                var new_roll = roll * 0.017453;
                var new_yaw = (yaw - 90) * 0.017453;

                var show_pitch = new_pitch - nodeBackUI.pitch;
                var show_roll  = new_roll - nodeBackUI.roll;
                var show_yaw  = new_yaw - nodeBackUI.yaw;

                // console.log("pitch:"+pitch+" nodeUI.pitch:"+nodeUI.pitch+" show_pitch:"+show_pitch);

                var xAxis = new THREE.Vector3(1, 0, 0);
                rotateAroundWorldAxis(nodeBackUI, xAxis, show_pitch);// pitch

                var yAxis = new THREE.Vector3(0, 1, 0);
                rotateAroundWorldAxis(nodeBackUI, yAxis, show_roll);// roll

                var zAxis = new THREE.Vector3(0, 0, 1);
                rotateAroundWorldAxis(nodeBackUI, zAxis, show_yaw);// yaw

                nodeBackUI.pitch = new_pitch;
                nodeBackUI.roll = new_roll;
                nodeBackUI.yaw = new_yaw;

                nodeBackUI.position.x = parseInt(nodeData[5]);
                if ( yaw < 0)
                    nodeBackUI.position.y = parseInt(nodeData[6]) + 1;
                else
                    nodeBackUI.position.y = parseInt(nodeData[6]) - 1;

                if (pitch < 0)
                    nodeBackUI.position.z = parseInt(nodeData[7]) + 1;
                else
                    nodeBackUI.position.z = parseInt(nodeData[7]) - 1;
            }

        }
        else
        {
            // found a new node
            var curColor = getAoAColor(nodeData[0]);
            AoANodeColorList[nodeData[0]] = curColor;

            var pitch = parseInt(nodeData[8]);
            var roll  = parseInt(nodeData[9]);
            var yaw  = parseInt(nodeData[10]);

            console.log(nodeData[0] + " pitch:"+pitch+" roll:"+roll+" yaw:"+yaw);

            //==========================================
            // var nodeOpacity = 0.9;
            // var material = new THREE.MeshBasicMaterial( {color: curColor, opacity: nodeOpacity} );
            // material.transparent = true;
            //
            // var circleSize = 10;
            // nodeUI = new THREE.Mesh( new THREE.SphereGeometry( circleSize, 10, 10 ), material );
            // //nodeUI.position.set( posX, posY, posZ );
            // nodeUI.visible = DrawAoAs;
            // scene.add( nodeUI );
            //==========================================
            var anchor_image = "/images/aoa_bottom.png";
            var textureLoader = new THREE.TextureLoader();
            var anchorTextureUV = textureLoader.load(anchor_image);
            anchorTextureUV.wrapS = anchorTextureUV.wrapT = THREE.RepeatWrapping;
            anchorTextureUV.anisotropy = 16;
            var anchorImageUV = new THREE.MeshBasicMaterial( { map: anchorTextureUV, transparent: true, side: THREE.DoubleSide } );
            nodeUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 235), anchorImageUV);

            var show_pitch = (pitch + 90) * 0.017453;
            var show_roll = roll * 0.017453;
            var show_yaw = (yaw - 90) * 0.017453;

            var xAxis = new THREE.Vector3(1, 0, 0);
            rotateAroundWorldAxis(nodeUI, xAxis, show_pitch);// pitch

            var yAxis = new THREE.Vector3(0, 1, 0);
            rotateAroundWorldAxis(nodeUI, yAxis, show_roll);// roll

            var zAxis = new THREE.Vector3(0, 0, 1);
            rotateAroundWorldAxis(nodeUI, zAxis, show_yaw);// yaw

            nodeUI.pitch = show_pitch;
            nodeUI.roll = show_roll;
            nodeUI.yaw = show_yaw;

            nodeUI.position.x = parseInt(nodeData[5]);
            nodeUI.position.y = parseInt(nodeData[6]);
            nodeUI.position.z = parseInt(nodeData[7]);

            scene.add( nodeUI );
            //==========================================
            var aoa_top_image = "/images/aoa_top.png";
            var aoa_top_textureLoader = new THREE.TextureLoader();
            var aoa_top_TextureUV = aoa_top_textureLoader.load(aoa_top_image);
            aoa_top_TextureUV.wrapS = aoa_top_TextureUV.wrapT = THREE.RepeatWrapping;
            aoa_top_TextureUV.anisotropy = 16;
            var aoa_top_ImageUV = new THREE.MeshBasicMaterial( { map: aoa_top_TextureUV, transparent: true, side: THREE.DoubleSide } );
            nodeBackUI = new THREE.Mesh(new THREE.PlaneGeometry(200, 235), aoa_top_ImageUV);
            nodeBackUI.position.x = 0;
            nodeBackUI.position.y = 0;
            nodeBackUI.position.z = 0;

            var show_pitch = (pitch + 90) * 0.017453;
            var show_roll = roll * 0.017453;
            var show_yaw = (yaw - 90) * 0.017453;

            var aoa_top_xAxis = new THREE.Vector3(1, 0, 0);
            rotateAroundWorldAxis(nodeBackUI, aoa_top_xAxis, show_pitch);// pitch

            var aoa_top_yAxis = new THREE.Vector3(0, 1, 0);
            rotateAroundWorldAxis(nodeBackUI, aoa_top_yAxis, show_roll);// roll

            var aoa_top_zAxis = new THREE.Vector3(0, 0, 1);
            rotateAroundWorldAxis(nodeBackUI, aoa_top_zAxis, show_yaw);// yaw

            nodeBackUI.pitch = show_pitch;
            nodeBackUI.roll = show_roll;
            nodeBackUI.yaw = show_yaw;

            nodeBackUI.position.x = parseInt(nodeData[5]);
            if ( yaw < 0)
                nodeBackUI.position.y = parseInt(nodeData[6]) + 1;
            else
                nodeBackUI.position.y = parseInt(nodeData[6]) - 1;

            if (pitch < 0)
                nodeBackUI.position.z = parseInt(nodeData[7]) + 1;
            else
                nodeBackUI.position.z = parseInt(nodeData[7]) - 1;


            scene.add( nodeBackUI );
            //==========================================

            var nodeinfo = {};
            nodeinfo['nodeid'] = nodeData[0];
            nodeinfo['nodename'] = decodeURIComponent(nodeData[1]);
            nodeinfo['posX'] = nodeData[5];
            nodeinfo['posY'] = nodeData[6];
            nodeinfo['posZ'] = nodeData[7];
            nodeinfo['nodetype'] = 'AoA';
            nodeinfo['nodedata'] = nodeData;

            //
            // add to list
            //
            AoAList.push(nodeUI);
            AoABackList.push(nodeBackUI);
            AoAInfoList.push(nodeinfo);

            //
            // line
            //
            var geometryline = new THREE.Geometry();
            geometryline.vertices.push( new THREE.Vector3( nodeinfo['posX'], nodeinfo['posY'], nodeinfo['posZ']) );
            geometryline.vertices.push( new THREE.Vector3( nodeinfo['posX'], nodeinfo['posY'], 0));
            var lineUI = new THREE.Line( geometryline, new THREE.LineBasicMaterial( { color: 0x8855ff, opacity: 0.6 } ) );
            AoALineList.push(lineUI);

            scene.add( lineUI );
        }
    }

    var rotWorldMatrix;
    // Rotate an object around an arbitrary axis in world space
    function rotateAroundWorldAxis(object, axis, radians)
    {
        rotWorldMatrix = new THREE.Matrix4();
        rotWorldMatrix.makeRotationAxis(axis.normalize(), radians);

        // old code for Three.JS pre r54:
        //  rotWorldMatrix.multiply(object.matrix);
        // new code for Three.JS r55+:
        rotWorldMatrix.multiply(object.matrix);                // pre-multiply

        object.matrix = rotWorldMatrix;

        // old code for Three.js pre r49:
        // object.rotation.getRotationFromMatrix(object.matrix, object.scale);
        // old code for Three.js pre r59:
        // object.rotation.setEulerFromRotationMatrix(object.matrix);
        // code for r59+:
        object.rotation.setFromRotationMatrix(object.matrix);
    }

    //
    // update map
    //
    function fetchMapGround()
    {
        var targetURL = gl_target_server + "/php/map.php";
        $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
        {
            var data = mapsdata[0];

            var magic_number = data.SCALE / 56.0736;
            var map3d_scale = 1.983 / magic_number;

            //================================
            // current user project  map
            var textureLoader = new THREE.TextureLoader();
            projectMapTextureUV = textureLoader.load(data.FILE);
            projectMapTextureUV.wrapS = projectMapTextureUV.wrapT = THREE.RepeatWrapping;
            projectMapTextureUV.anisotropy = 16;
            projectMapUV = new THREE.MeshBasicMaterial( { map: projectMapTextureUV } );
            //================================


            //================================
            // Ground map
            var map_width = data.MAPWIDTH * map3d_scale;
            var map_height = data.MAPHEIGHT * map3d_scale;
            var geo = new THREE.PlaneBufferGeometry(map_width, map_height, 8, 8);
            var mat = new THREE.MeshBasicMaterial({ map: projectMapTextureUV, transparent: true, side: THREE.DoubleSide });
            GroudPlane = new THREE.Mesh(geo, mat);
            scene.add(GroudPlane);
            //================================

            // origin point
            var origin_POSX = data.POSX;
            var origin_POSY = data.POSY;

            GroudPlane.position.set(map_width/2 - origin_POSX, map_height/2 - origin_POSY, 0 );
            camera.position.set(map_width/2 - origin_POSX, -MAX_AXIS/4, MAX_AXIS/2);

            camera_lookat_point = new THREE.Vector3(map_width/2 - origin_POSX, 0, 0);
            controls.target.set(camera_lookat_point.x, camera_lookat_point.y, camera_lookat_point.z);
            controls.update();

        });
        return false;
    }

    function fetchTagsnewloc()
    {
        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
        {
            gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
            gl_tag_num = parseInt(data['LOC_TAG_NUM']);
            gl_coord_num = parseInt(data['LOC_COORD_NUM']);
            gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);
            gl_aoa_num = parseInt(data['LOC_AOA_NUM']);

            gl_anchor_load_index = 0;
            gl_tag_load_index = 0;
            gl_coord_load_index = 0;
            gl_locator_load_index = 0;
            gl_aoa_load_index = 0;
        })
        .success(function() {
            setTimeout(runLive_tag, 10);
        })
        .error(function() {
            setTimeout(fetchTagsnewloc, 1000);
        })
        .complete(function() {
        });
    }

    function runLive_tag()
    {

        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            setTimeout(runLive_tag, 500);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
        {
            var tag_num = parseInt(data['LOC_TAG_NUM']);

            //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

            for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
            {
                var key = 'LOC_TAG_INDEX_'+i;
                if (data[key] == undefined || data[key] == null)
                    continue;
                var node = data[key];
                var nodeData = node.split(",");
                update_node_to_ui('Tag', TagInfoList, TagList, TagLineList, nodeData);
            }

            gl_tag_load_index += tag_num;

        })
        .success(function() {
            if (gl_tag_load_index >= gl_tag_num)
            {
                gl_tag_load_index = 0;
                setTimeout(runLive_tag, 500);
            }
            else
            {
                runLive_tag();
            }
        })
        .error(function() {
            gl_tag_load_index = 0;
            setTimeout(runLive_tag, 500);
        })
        .complete(function() {
        });
        //setTimeout(function(){ p.abort(); }, 5000);

    }

    // function fetchTagsnewloc_old()
    // {
    //     var targetURL = gl_target_server + "/php/vilsnodes.php";
    //
    //     $.getJSON(targetURL, 'loadtag=1'+'&project_id='+project_id, function(data)
    //     {
    //
    //         //
    //         // Server devices
    //         //
    //         var loc_coord_num = parseInt(data['LOC_COORD_NUM']);
    //         var loc_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
    //         var loc_tag_num = parseInt(data['LOC_TAG_NUM']);
    //
    //         //alert('fetchNodes() loc_anchor_num='+ loc_anchor_num);
    //
    //         for (var i = 0; i < loc_tag_num; ++i)
    //         {
    //             var key = 'LOC_TAG_INDEX_'+i;
    //             if (data[key] == undefined || data[key] == null)
    //                 continue;
    //
    //             var node = data[key];
    //             var nodeData = node.split(",");
    //             update_node_to_ui('Tag', TagInfoList, TagList, TagLineList, nodeData);
    //          }
    //     })
    //     .complete(function() { setTimeout(fetchTagsnewloc, 10); });
    // }

    function fetchAnchors()
    {
        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            setTimeout(fetchAnchors, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
        {
            var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

            //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

            for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
            {
                var key = 'LOC_ANCHOR_INDEX_'+i;
                if (data[key] == undefined || data[key] == null)
                    continue;

                var node = data[key];
                var nodeData = node.split(",");
                update_node_to_ui('Anchor', AnchorInfoList, AnchorList, AnchorLineList, nodeData);
            }

            gl_anchor_load_index += anchor_num;

        })
        .success(function() {
            if (gl_anchor_load_index >= gl_anchor_num)
            {
                gl_anchor_load_index = 0;
                setTimeout(fetchAnchors, 10000);
            }
            else
            {
                setTimeout(fetchAnchors, 10);
            }
        })
        .error(function() {
            gl_anchor_load_index = 0;
            setTimeout(fetchAnchors, 30000);
        })
        .complete(function() {
        });
        //setTimeout(function(){ p.abort(); }, 5000);

    }

    function fetchCoordinators()
    {
        if (gl_coord_load_index >= gl_coord_num)
        {
            gl_coord_load_index = 0;
            setTimeout(fetchCoordinators, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadcoord=1&s='+gl_coord_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
        {
            var coord_num = parseInt(data['LOC_COORD_NUM']);

            for (var i = gl_coord_load_index; i < gl_coord_load_index + coord_num; ++i)
            {
                var key = 'LOC_COORD_INDEX_'+i;
                if (data[key] == undefined || data[key] == null)
                    continue;

                var node = data[key];
                var nodeData = node.split(",");
                update_node_to_ui('Coordinator', CoordinatorInfoList, CoordinatorList, CoordinatorLineList, nodeData);
            }

            gl_coord_load_index += coord_num;

        })
        .success(function() {
            if (gl_coord_load_index >= gl_coord_num)
            {
                gl_coord_load_index = 0;
                setTimeout(fetchCoordinators, 10000);
            }
            else
            {
                setTimeout(fetchCoordinators, 10);
            }
        })
        .error(function() {
            gl_coord_load_index = 0;
            setTimeout(fetchCoordinators, 30000);
        })
        .complete(function() {
        });
    }

    function fetchLocators()
    {
        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;
            setTimeout(fetchLocators, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
        {
            var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

            for (var i = gl_locator_load_index; i < gl_locator_load_index + locator_num; ++i)
            {
                var key = 'LOC_LOCATOR_INDEX_'+i;
                if (data[key] == undefined || data[key] == null)
                    continue;

                var node = data[key];
                var nodeData = node.split(",");
                update_node_to_ui('Locator', LocatorInfoList, LocatorList, LocatorLineList, nodeData);
            }

            gl_locator_load_index += locator_num;

        })
        .success(function() {
            if (gl_locator_load_index >= gl_locator_num)
            {
                gl_locator_load_index = 0;
                setTimeout(fetchLocators, 10000);
            }
            else
            {
                setTimeout(fetchLocators, 10);
            }
        })
        .error(function() {
            gl_locator_load_index = 0;
            setTimeout(fetchLocators, 30000);
        })
        .complete(function() {
        });
    }

    function fetchAoAs()
    {
        if (gl_aoa_load_index >= gl_aoa_num)
        {
            gl_aoa_load_index = 0;
            setTimeout(fetchAoAs, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadaoa=1&s='+gl_aoa_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
        {
            var aoa_num = parseInt(data['LOC_AOA_NUM']);

            for (var i = gl_aoa_load_index; i < gl_aoa_load_index + aoa_num; ++i)
            {
                var key = 'LOC_AOA_INDEX_'+i;
                if (data[key] == undefined || data[key] == null)
                    continue;

                var node = data[key];
                var nodeData = node.split(",");
                update_aoa_node_to_ui(nodeData);
            }

            gl_aoa_load_index += aoa_num;

        })
        .success(function() {
            if (gl_aoa_load_index >= gl_aoa_num)
            {
                gl_aoa_load_index = 0;
                if (glTestAoAPowerTag)
                    setTimeout(fetchAoAs, 500);
                else
                    setTimeout(fetchAoAs, 5000);
            }
            else
            {
                setTimeout(fetchAoAs, 10);
            }
        })
        .error(function() {
            gl_aoa_load_index = 0;
            setTimeout(fetchAoAs, 30000);
        })
        .complete(function() {
        });
    }

    function clear_AoARangingCloudPoints()
    {
        for(key in AoARangingCloudPointList)
        {
            var curUIList = AoARangingCloudPointList[key];
            for(var index=0;index<curUIList.length;index++)
            {
                var sphere = curUIList[index];
                scene.remove(sphere);
                sphere = undefined;
                delete(sphere);
            }
            AoARangingCloudPointList[key] = [];
        }
    }

    function clear_AoAAngleVectorCloudPoints()
    {
        for(key in AoAAngleVectorCloudPointList)
        {
            var curUIList = AoAAngleVectorCloudPointList[key];
            for(var index=0;index<curUIList.length;index++)
            {
                var sphere = curUIList[index];
                scene.remove(sphere);
                sphere = undefined;
                delete(sphere);
            }
            AoAAngleVectorCloudPointList[key] = [];
        }
    }

    function clear_AoAAngleCloudPoints()
    {
        for(key in AoAAngleCloudPointList)
        {
            var curUIList = AoAAngleCloudPointList[key];
            for(var index=0;index<curUIList.length;index++)
            {
                var sphere = curUIList[index];
                scene.remove(sphere);
                sphere = undefined;
                delete(sphere);
            }
            AoAAngleCloudPointList[key] = [];
        }
    }

    function fetchAoARangingCloudPoint()
    {
        if (DrawAoARangingCloudPoints != true)
        {
            // clear old ui
            clear_AoARangingCloudPoints();
            setTimeout(fetchAoARangingCloudPoint, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadaoarangingcloudpoint=1&project_id='+project_id, function(data)
        {
            if (data.tag != undefined)
            {
                // console.log("Tag:"+data.tag);

                var curUIList = AoARangingCloudPointList[data.tag];
                if (curUIList != undefined)
                {
                    // clear old ui
                    for(var index=0;index<curUIList.length;index++)
                    {
                        var sphere = curUIList[index];
                        scene.remove(sphere);
                        sphere = undefined;
                        delete(sphere);
                    }
                    AoARangingCloudPointList[data.tag] = [];
                }
                else
                {
                    AoARangingCloudPointList[data.tag] = [];
                }

                // if (data.result != undefined)
                //     console.log(" result["+data.result[0]+","+data.result[1]+","+data.result[2]+"]");

                if (data.data != undefined)
                {
                    // console.log(" data.data.length:"+data.data.length);
                    for(var index=0;index<data.data.length;index++)
                    {
                        var aoaCloudPoints = data.data[index];
                        //console.log("aoa:"+aoaCloudPoints.aoa+" points.length:"+aoaCloudPoints.points.length);
                        var circleSize = 5;
                        // var colorIndex = index;
                        // if (colorIndex >= 10)
                        //     colorIndex -= 10;
                        var pointList = create_cloud_point(aoaCloudPoints.points, getAoAColor(aoaCloudPoints.aoa), circleSize, 0.5);

                        AoARangingCloudPointList[data.tag].push.apply(AoARangingCloudPointList[data.tag], pointList);
                    }
                }
            }
        })
        .complete(function() { setTimeout(fetchAoARangingCloudPoint, 1000); })
        .error(function() { setTimeout(fetchAoARangingCloudPoint, 5000); });
    }

    function fetchAoAAngleVectorCloudPoint()
    {
        if (DrawAoAAngleVectorCloudPoints != true)
        {
            clear_AoAAngleVectorCloudPoints();
            setTimeout(fetchAoAAngleVectorCloudPoint, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadaoaanglevectorcloudpoint=1&project_id='+project_id, function(data)
        {
            if (data.tag != undefined)
            {
                // console.log("Tag:"+data.tag);
                var curUIList = AoAAngleVectorCloudPointList[data.tag];
                if (curUIList != undefined)
                {
                    // clear old ui
                    for(var index=0;index<curUIList.length;index++)
                    {
                        var sphere = curUIList[index];
                        scene.remove(sphere);
                        sphere = undefined;
                        delete(sphere);
                    }
                    AoAAngleVectorCloudPointList[data.tag] = [];
                }
                else
                {
                    AoAAngleVectorCloudPointList[data.tag] = [];
                }

                //
                // if (data.result != undefined)
                //     console.log(" result["+data.result[0]+","+data.result[1]+","+data.result[2]+"]");


                if (data.data != undefined)
                {
                    //console.log(" data.data.length:"+data.data.length);
                    for(var index=0;index<data.data.length;index++)
                    {
                        var aoaCloudPoints = data.data[index];
                        // console.log("aoa:"+aoaCloudPoints.aoa+" points.length:"+aoaCloudPoints.points.length);

                        // find AoA point
                        var AoAnode = getAoANode(aoaCloudPoints.aoa);
                        // console.log("posX:"+AoAnode['posX']+" posY:"+AoAnode['posY']);

                        // var colorIndex = index;
                        // if (colorIndex >= 10)
                        //     colorIndex -= 10;
                        if (AoAnode == null)
                        {
                            var circleSize = 5;
                            var pointList = create_cloud_point(aoaCloudPoints.points, getAoAColor(aoaCloudPoints.aoa), circleSize, 0.3);
                            AoAAngleVectorCloudPointList[data.tag].push.apply(AoAAngleVectorCloudPointList[data.tag], pointList);
                        }
                        else
                        {
                            var pointList = create_cloud_vector(AoAnode, aoaCloudPoints.points, getAoAColor(aoaCloudPoints.aoa), 0.05);
                            AoAAngleVectorCloudPointList[data.tag].push.apply(AoAAngleVectorCloudPointList[data.tag], pointList);
                        }
                    }
                }
            }
        })
        .complete(function() { setTimeout(fetchAoAAngleVectorCloudPoint, 1000); })
        .error(function() { setTimeout(fetchAoAAngleVectorCloudPoint, 5000); });
    }

    function fetchAoAAngleCloudPoint()
    {
        if (DrawAoAAngleCloudPoints != true)
        {
            clear_AoAAngleCloudPoints();
            setTimeout(fetchAoAAngleCloudPoint, 1000);
            return;
        }

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadaoaanglecloudpoint=1&project_id='+project_id, function(data)
        {
            if (data.tag != undefined)
            {
                //console.log("Tag:"+data.tag);
                var curUIList = AoAAngleCloudPointList[data.tag];
                if (curUIList != undefined)
                {
                    // clear old ui
                    for(var index=0;index<curUIList.length;index++)
                    {
                        var sphere = curUIList[index];
                        scene.remove(sphere);
                        sphere = undefined;
                        delete(sphere);
                    }
                    AoAAngleCloudPointList[data.tag] = [];
                }
                else
                {
                    AoAAngleCloudPointList[data.tag] = [];
                }

                //
                // if (data.result != undefined)
                //     console.log(" result["+data.result[0]+","+data.result[1]+","+data.result[2]+"]");


                if (data.data != undefined)
                {
                    // console.log(" data.data.length:"+data.data.length);
                    for(var index=0;index<data.data.length;index++)
                    {
                        var aoaCloudPoints = data.data[index];
                        // console.log("aoa:"+aoaCloudPoints.aoa+" points.length:"+aoaCloudPoints.points.length);
                        var circleSize = 5;
                        // var colorIndex = index;
                        // if (colorIndex >= 10)
                        //     colorIndex -= 10;

                        var pointList = create_cloud_point(aoaCloudPoints.points, getAoAColor(aoaCloudPoints.aoa1), circleSize, 0.3);
                        //var pointList = create_cloud_point(aoaCloudPoints.points, ColorsTable[colorIndex], circleSize, 0.3);

                        AoAAngleCloudPointList[data.tag].push.apply(AoAAngleCloudPointList[data.tag], pointList);
                    }
                }
            }
        })
        .complete(function() { setTimeout(fetchAoAAngleCloudPoint, 1000); })
        .error(function() { setTimeout(fetchAoAAngleCloudPoint, 5000); });
    }

    function getAoANode(nodeid)
    {
        var AoAnode = null;
        for(var index=0;index<AoAInfoList.length;index++)
        {
            var node = AoAInfoList[index];
            if ( node['nodeid'] === nodeid )
            {
                AoAnode = node;
                break;
            }
        }

        return AoAnode;
    }

    function getAoAColor(nodeid)
    {
        var curColor = AoANodeColorList[nodeid];
        if ( curColor != undefined )
                return curColor;

        var colorindex = (Object.keys(AoANodeColorList).length + 0)%10;
        curColor = ColorsTable[colorindex];
        AoANodeColorList[nodeid] = curColor;

        //console.log("nodeid:"+nodeid+" colorindex:"+colorindex);

        return curColor;
    }

    function onWindowResize()
    {
        var loShowAspect = $("#content").height() / $("#content").width() ;

        camera.aspect = 1/loShowAspect;
        camera.updateProjectionMatrix();
        renderer.setSize( $("#content").width(), $("#content").height());
    }

    function onDocumentMouseMove( event )
    {
       event.preventDefault();

        var offsets = $('#content').offset();
        var top = offsets.top;
        var left = offsets.left;

        var showWidth = $("#content").width();
        var showHeight = $("#content").height();

        // check if headless
        var top_shift = 70;
        if ( document.getElementById("vilslogo") == null )
            top_shift = 0;

        mouse.x = ( (event.clientX - offsets.left - MARGIN) / showWidth ) * 2 - 1;
        mouse.y = - ( (event.clientY - offsets.top - top_shift) / showHeight ) * 2 + 1;

        scale_nodes(TagList, TagInfoList);
        scale_nodes(AnchorList, AnchorInfoList);
    }

    function scale_nodes(NodeList, NodeInfoList)
    {

        for	(index = 0; index < NodeList.length; index++)
        {
            var tag = NodeInfoList[index];
            if ( tag['nodename'] == "WilliamWu")
                continue;

            //cameara.z; 1000
            if ( camera.position.z <= 1000.0 )
            {
                NodeList[index].scale.set( 0.3, 0.3, 0.3 );
            }
            else
            if ( camera.position.z <= 1500.0 )
            {
                NodeList[index].scale.set( 0.5, 0.5, 0.5 );
            }
            else
                NodeList[index].scale.set( 1.0, 1.0, 1.0);
        }
    }

    //
    // check object intersect
    //
    function object_intersect(ObjList, ObjInfoList)
    {
        //var intersects = raycaster.intersectObjects( scene.children );
        var intersects = raycaster.intersectObjects( ObjList );
        if ( intersects.length > 0 )
        {
            if ( INTERSECTED != intersects[ 0 ].object )
            {
                if ( INTERSECTED )
                    INTERSECTED.material.color.setHex( INTERSECTED.currentHex );

                //var materialVILS = new THREE.MeshBasicMaterial( { map: textureVILS } );
                INTERSECTED = intersects[ 0 ].object;
                INTERSECTED.currentHex = INTERSECTED.material.color.getHex();
                //INTERSECTED.material.emissive.setHex( 0xff0000 );
                INTERSECTED.material.color.setHex(0xffff00);

                //var pos = INTERSECTED.matrixWorld.getPosition();
                var pos = new THREE.Vector3();
                pos.setFromMatrixPosition( INTERSECTED.matrixWorld );
                objectSphere.position.set( pos.x, pos.y, pos.z);
                objectSphere.visible = true;

                //
                // show node info
                //
                for	(index = 0; index < ObjList.length; index++)
                {
                    if ( ObjList[index] == INTERSECTED )
                    {
                        if (index < ObjInfoList.length)
                        {
                            glSelectedNode = ObjInfoList[index];
                            document.getElementById("nodename").innerHTML = glSelectedNode.nodename;
                            document.getElementById("nodepos").innerHTML =
                            "("+glSelectedNode.posX+", " + glSelectedNode.posY+", " + glSelectedNode.posZ+")";

                            // if ( parent.$('#rightcontentIframe')[0] != undefined )
                            // {
                            //     if ( typeof parent.$('#rightcontentIframe')[0].contentWindow.show_device_info === "function")
                            //     {
                            //         parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID = glSelectedNode.nodeid;
                            //         parent.$('#rightcontentIframe')[0].contentWindow.show_device_info(glSelectedNode.nodetype, glSelectedNode['nodedata']);
                            //     }
                            // }
                        }
                    }
                }
            }
        }
        else
        {
            //if ( INTERSECTED )
            //    INTERSECTED.material.color.setHex( INTERSECTED.currentHex );
            //INTERSECTED = null;
        }
        return intersects.length;
    }


    function animate()
    {

        requestAnimationFrame( animate );

        camera.updateProjectionMatrix();

        rotate_nodes(TagList, TagRotationList, TagLineList, DrawTags, true);
        rotate_nodes(AnchorList, AnchorRotationList, AnchorLineList, DrawAnchors, true);
        rotate_nodes(CoordinatorList, CoordinatorRotationList, CoordinatorLineList, DrawCoordinators, true);
        rotate_nodes(LocatorList, LocatorRotationList, LocatorLineList, DrawLocators, true);
        rotate_nodes(AoAList, AoARotationList, AoALineList, DrawAoAs, false);

        for	(var index = 0; index < AoABackList.length; index++)
        {
            AoABackList[index].visible = DrawAoAs;
        }

        if (GroudPlane != undefined)
        {
            GroudPlane.visible = DrawMap;
        }

        render();
    }

    function rotate_nodes(NodeList, NodeRotationList, NodeLineList, showNode, enableRotate)
    {
        for	(var index = 0; index < NodeList.length; index++)
        {
            if (enableRotate)
            {
                if (NodeRotationList[index] == undefined || NodeRotationList[index] == null)
                {
                    NodeRotationList[index] = 0.0;
                }
                else
                {
                    NodeRotationList[index] += 0.01;
                }
                NodeList[index].rotation.y += 0.01;
            }
            NodeList[index].visible = showNode;
            NodeLineList[index].visible = showNode;
        }
    }

    function render()
    {

        // find intersections
        raycaster.setFromCamera( mouse, camera );

        //
        // check anchor intersect
        //
        var icount = object_intersect(AnchorList, AnchorInfoList);

        //
        // check tag intersect
        //
        icount += object_intersect(TagList, TagInfoList);
        if (icount == 0)
        {
            if ( INTERSECTED )
                INTERSECTED.material.color.setHex( INTERSECTED.currentHex );
            INTERSECTED = null;
            objectSphere.visible = false;
        }


        renderer.render( scene, camera );

    }


    //
    // create fadeout foot print
    //
    function add_new_footprint(node_id, posX, posY, posZ)
    {

        var curColor = FadeOutNodeColors[node_id];
        if (curColor == null || curColor == undefined)
            curColor = ColorsTable[(Object.keys(FadeOutNodeColors).length)%10];
        FadeOutNodeColors[node_id] = curColor;

        //alert('new curColor=' + curColor +', node_id='+node_id +', FadeOutNodeColors.length='+FadeOutNodeColors.length);
        var circleSize = 40;
        if ( camera.position.z <= 1000.0 )
        {
            circleSize = 10;
        }
        else
        if ( camera.position.z <= 1500.0 )
        {
            circleSize = 20;
        }

        //
        // sphere
        //
        var material = new THREE.MeshBasicMaterial( {color: curColor, opacity: 0.5} );
        material.transparent = true;
        var sphereFT = new THREE.Mesh( new THREE.SphereGeometry( circleSize, 10, 10 ), material );
        sphereFT.position.set( posX, posY, posZ );
        sphereFT.visible = DrawTags;
        scene.add( sphereFT );

        var TagFadeOutNodes = FadeOutNodes[node_id];
        if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
            TagFadeOutNodes = [];
        TagFadeOutNodes[ TagFadeOutNodes.length ] = sphereFT;
        FadeOutNodes[node_id] = TagFadeOutNodes;

        //alert('node_id='+node_id +', TagFadeOutNodes.length='+TagFadeOutNodes.length);

    }


    //
    // fade out circle
    //
    function fadeout_tag_footprint(tag_node_id)
    {
        var existFadeOutNodes = [];

        var TagFadeOutNodes = FadeOutNodes[tag_node_id];

        if (TagFadeOutNodes == null || TagFadeOutNodes == undefined)
            return;

        for(var i=0;i<TagFadeOutNodes.length;i++)
        {
            var sphere = TagFadeOutNodes[i];

            sphere.visible = DrawTags;

            //radius = sphere.radius;
            //var scale = radius * 1.1; // adjust the multiplier to whatever
            sphere.scale.x = sphere.scale.x * 0.8;
            sphere.scale.y = sphere.scale.y * 0.8;
            sphere.scale.z = sphere.scale.z * 0.8;

            if ( sphere.scale.x <= 0.1 )
            {
                // release resource
                sphere.geometry.dispose();
                sphere.material.dispose();
                scene.remove(sphere);
                sphere = undefined;
                delete(sphere);
            }
            else
            {
                // put back to list
                existFadeOutNodes[ existFadeOutNodes.length ] = sphere;
            }
        }
        TagFadeOutNodes = undefined;
        FadeOutNodes[tag_node_id] = existFadeOutNodes;
    }

    function fadeout_loop()
    {
        for	(index = 0; index < TagInfoList.length; index++)
        {
            tag_node = TagInfoList[index];
            fadeout_tag_footprint(tag_node.nodeid);
        }

        setTimeout(fadeout_loop, 500);
    }

    function update_AoAPowerTagInfo(once)
    {
        if (glTestAoAPowerTag)
        {

            var jsondata = {};
            jsondata['project_id'] = ''+project_id;
            jsondata['updateAoAPowerTagInfo'] = 1;
            jsondata['AoAPowerTag_1'] = document.getElementById("testAoAPowerTag_1_InputText").value;
            jsondata['AoAPowerTag_2'] = document.getElementById("testAoAPowerTag_2_InputText").value;
            jsondata['AoAPowerTag_3'] = document.getElementById("testAoAPowerTag_3_InputText").value;
            jsondata['AoAPowerTag_aoaid'] = document.getElementById("testAoAPowerTag_aoaid_InputText").value;
            var targetURL = gl_target_server + "/php/vilsnodes.php";

            jQuery.ajax({
              url: targetURL,
              type: 'POST',
              data: jsondata,
              success:
                function(response)
                {
                },
              error:
                function(xhr, status, error)
                {
                    alert("error = " + error);
                    alert("xhr.responseText = " + xhr.responseText);
                }
           });
        }

        if (!once)
            setTimeout(update_AoAPowerTagInfo, 30000, once);
    }


    //
    // move camera position
    //
    document.onkeydown = function(e)
    {
        switch (e.keyCode)
        {
            case 37:
                //alert('left');
                //camera.position.x = camera.position.x - camera_move_delta;
                camera_lookat_point.x = camera_lookat_point.x - camera_move_delta;
                break;
            case 38:
                //alert('up');
                //camera.position.z = camera.position.z - camera_move_delta;
                camera_lookat_point.z = camera_lookat_point.z - camera_move_delta;
                break;
            case 39:
                //alert('right');
                //camera.position.x = camera.position.x + camera_move_delta;
                camera_lookat_point.x = camera_lookat_point.x + camera_move_delta;
                break;
            case 40:
                //alert('down');
                //camera.position.z = camera.position.z + camera_move_delta;
                camera_lookat_point.z = camera_lookat_point.z + camera_move_delta;
                break;
        }
        controls.target.set(camera_lookat_point.x, camera_lookat_point.y, camera_lookat_point.z);
    };

    $("#showanchors").click(function()
    {
        DrawAnchors = document.getElementById('showanchors').checked;
        setCookie("DrawAnchors", DrawAnchors+'', 10);
        return true;
    });

    $("#showcoordinators").click(function()
    {
        DrawCoordinators = document.getElementById('showcoordinators').checked;
        setCookie("DrawCoordinators", DrawCoordinators+'', 10);
        return true;
    });

    $("#showlocators").click(function()
    {
        DrawLocators = document.getElementById('showlocators').checked;
        setCookie("DrawLocators", DrawLocators+'', 10);
        return true;
    });

    $("#showtags").click(function()
    {
        DrawTags = document.getElementById('showtags').checked;
        setCookie("DrawTags", DrawTags+'', 10);
        return true;
    });

    $("#showaoas").click(function()
    {
        DrawAoAs = document.getElementById('showaoas').checked;
        setCookie("DrawAoAs", DrawAoAs+'', 10);
        return true;
    });

    $("#showmap").click(function()
    {
        DrawMap = document.getElementById('showmap').checked;
        setCookie("DrawMap", DrawMap+'', 10);
        return true;
    });

    $("#showAoARangingCloudPoints").click(function()
    {
        DrawAoARangingCloudPoints = document.getElementById('showAoARangingCloudPoints').checked;
        setCookie("DrawAoARangingCloudPoints", DrawAoARangingCloudPoints+'', 10);
        return true;
    });

    $("#showAoAAngleVectorCloudPoints").click(function()
    {
        DrawAoAAngleVectorCloudPoints = document.getElementById('showAoAAngleVectorCloudPoints').checked;
        setCookie("DrawAoAAngleVectorCloudPoints", DrawAoAAngleVectorCloudPoints+'', 10);
        return true;
    });

    $("#showAoAAngleCloudPoints").click(function()
    {
        DrawAoAAngleCloudPoints = document.getElementById('showAoAAngleCloudPoints').checked;
        setCookie("DrawAoAAngleCloudPoints", DrawAoAAngleCloudPoints+'', 10);
        return true;
    });

    $("#testAoAPowerTag").click(function()
    {
        glTestAoAPowerTag = document.getElementById('testAoAPowerTag').checked;
        setCookie("enableTestAoAPowerTag", glTestAoAPowerTag+'', 10);

        if(glTestAoAPowerTag)
            document.getElementById("testAoAPowerTag_ul").style.display = 'block';
        else
            document.getElementById("testAoAPowerTag_ul").style.display = 'none';

        return true;
    });

    $("#updateAoAPowerTagInfobtn").click(function()
    {
        setCookie("testAoAPowerTag_1", document.getElementById("testAoAPowerTag_1_InputText").value, 10);
        setCookie("testAoAPowerTag_2", document.getElementById("testAoAPowerTag_2_InputText").value, 10);
        setCookie("testAoAPowerTag_3", document.getElementById("testAoAPowerTag_3_InputText").value, 10);
        setCookie("testAoAPowerTag_aoaid", document.getElementById("testAoAPowerTag_aoaid_InputText").value, 10);

        update_AoAPowerTagInfo(true);
    });

    function open_3dmap_page()
    {
        var win = window.open('/index.php/vils3dmap/'+project_id, '_blank');
        win.focus();
    }

    function setCookie(name, value, days)
    {
        var expires = "";
        if (days)
        {
            var date = new Date();
            date.setTime(date.getTime() + (days*24*60*60*1000));
            expires = "; expires=" + date.toUTCString();
        }
        document.cookie = name + "=" + (value || "")  + expires + "; path=/";
    }

    function getCookie(name)
    {
        var nameEQ = name + "=";
        var ca = document.cookie.split(';');
        for(var i=0;i < ca.length;i++)
        {
            var c = ca[i];
            while (c.charAt(0)==' ')
                c = c.substring(1,c.length);
            if (c.indexOf(nameEQ) == 0)
            {
                return c.substring(nameEQ.length,c.length);
            }
        }
        return null;
    }

    function eraseCookie(name)
    {
        document.cookie = name+'=; Max-Age=-99999999;';
    }

    function create_cloud_point(nodelist, nodecolor, circleSize, circleOpacity)
    {
        var uilist = [];
        if(nodelist == undefined)
            return uilist;
        for(var index=0;index<nodelist.length;index++)
        {
            var node = nodelist[index];
            var sphereFT = create_test_Sphere(node, nodecolor, circleSize, circleOpacity);
            uilist.push(sphereFT);

            // var material = new THREE.MeshBasicMaterial( {color: nodecolor, opacity: 0.5} );
            // material.transparent = true;
            // var sphereFT = new THREE.Mesh( new THREE.SphereGeometry( circleSize, 10, 10 ), material );
            // sphereFT.position.set( node[0], node[1], node[2] );
            // sphereFT.visible = true;
            // scene.add( sphereFT );
        }
        return uilist;
    }

    function create_cloud_vector(aoaNode, nodelist, linecolor, lineOpacity)
    {
        var uilist = [];
        if(nodelist == undefined)
        {
            console.log("create_cloud_vector() nodelist == undefined");
            return uilist;
        }

        //console.log("create_cloud_vector() nodelist.length:"+nodelist.length);
        //console.log("create_cloud_vector() aoaNode['posX']:"+aoaNode['posX'] + " aoaNode['posY']:"+aoaNode['posY'] + " aoaNode['posZ']:"+aoaNode['posZ']);


        for(var index=0;index<nodelist.length;index++)
        {
            var node = nodelist[index];

            //console.log("create_cloud_vector() node[0]:"+node[0] + " node[1]:"+node[1] + " node[2]:"+node[2]);

            var geometryline = new THREE.Geometry();
            geometryline.vertices.push( new THREE.Vector3( aoaNode['posX'], aoaNode['posY'], aoaNode['posZ']) );
            geometryline.vertices.push( new THREE.Vector3( node[0], node[1], node[2]));
            var lineUI = new THREE.Line( geometryline, new THREE.LineBasicMaterial( { color: linecolor, opacity: lineOpacity } ) );
            scene.add( lineUI );

            uilist.push(lineUI);
        }
        return uilist;
    }


    function create_test_nodes()
    {
    var testNodes1 = [

[-1474,389,380],[-1452,384,380],[-1431,380,380],[-1411,376,380],[-1391,372,380],[-1372,369,380],[-1353,365,380],[-1334,361,380],[-1316,358,380],[-1299,355,380],[-1282,351,380],[-1265,348,380],[-1249,345,380],[-1233,342,380],[-1218,339,380],[-1203,337,380],[-1188,334,380],[-1174,331,380],[-1159,329,380],[-1146,326,380],[-1132,324,380],[-1119,322,379],[-1106,319,380],[-1093,317,380],[-1081,315,380],[-1069,313,380],[-1057,311,380],[-1045,309,380],[-1034,307,380],[-1022,305,380],[-1011,303,380],[-1001,302,379],[-990,300,380],[-980,298,380],[-970,297,380],[-960,295,380],[-950,294,380],[-940,292,380],[-931,291,380],[-921,289,380],[-912,288,380],[-903,287,380],[-894,285,380],[-886,284,380],[-877,283,380],[-869,282,380],[-860,280,380],[-852,279,380],[-844,278,380],[-836,277,380],[-829,276,380],[-821,275,380],[-814,274,380],[-806,273,380],[-799,272,380],[-792,271,380],[-785,271,380],[-778,270,380],[-771,269,380],[-764,268,380],[-757,267,380],[-751,267,380],[-744,266,380],[-738,265,380],[-732,264,380],[-726,264,380],[-719,263,380],[-713,263,380],[-707,262,380],[-702,261,380],[-696,261,380],[-690,260,380],[-684,260,380],[-679,259,380],[-673,259,380],[-668,258,380],[-663,258,380],[-657,257,380],[-652,257,380],[-647,257,380],[-642,256,380],[-637,256,380],[-632,256,380],[-627,255,380],[-622,255,380],[-617,255,380],[-612,254,380],[-608,254,380],[-603,254,380],[-599,253,380],[-594,253,380],[-590,253,380],[-585,253,380],[-581,253,380],[-576,252,380],[-572,252,380],[-568,252,380],[-564,252,380],[-560,252,380],[-555,252,379],[-551,252,380],[-547,252,380],[-543,251,380],[-539,251,380],[-536,251,380],[-532,251,380],[-528,251,380],[-524,251,380],[-520,251,380],[-517,251,380],[-513,251,380],[-509,251,380],[-506,251,380],[-502,251,380],[-499,251,380],[-495,251,380],[-492,251,380],[-488,251,380],[-485,252,380],[-482,252,380],[-478,252,380],[-475,252,380],[-472,252,379],[-468,252,380],[-465,252,380],[-462,252,380],[-459,252,380],[-456,253,380],[-452,253,380],[-449,253,380],[-446,253,380],[-443,253,380],[-440,254,380],[-437,254,380],[-434,254,380],[-431,254,380],[-428,254,380],[-426,255,380],[-423,255,380],[-420,255,380],[-417,255,380],[-414,256,380],[-411,256,380],[-409,256,380],[-406,257,379],[-403,257,380],[-401,257,380],[-398,258,380],[-395,258,380],[-393,258,380],[-390,259,380],[-387,259,380],[-385,259,380],[-382,260,380],[-380,260,380],[-377,260,380],[-375,261,380],[-372,261,380],[-370,262,380],[-367,262,380],[-365,262,380],[-362,263,380],[-360,263,380],[-358,264,380],[-355,264,380],[-353,265,380],[-351,265,380],[-348,265,380],[-346,266,379],[-344,266,379],[-341,267,380],[-339,267,380],[-337,268,380],[-335,268,380],[-332,269,380],[-330,269,380],[-328,270,380],[-326,270,380],[-324,271,379],[-321,272,379],[-319,272,380],[-317,273,380],[-315,273,380],[-313,274,380],[-311,274,380],[-309,275,380],[-307,276,379],[-304,276,380],[-302,277,380],[-300,277,380],[-298,278,380],[-296,279,379],[-294,279,380],[-292,280,380],[-290,280,380],[-288,281,380],[-286,282,380],[-284,282,380],[-282,283,380],[-281,284,380],[-279,284,380],[-277,285,380],[-275,286,380],[-273,287,380],[-271,287,380],[-269,288,380],[-267,289,380],[-265,289,380],[-263,290,380],[-262,291,380],[-260,292,380],[-258,292,380],[-256,293,380],[-254,294,380],[-253,295,380],[-251,296,380],[-249,296,379],[-247,297,380],[-245,298,380],[-244,299,380],[-242,300,380],[-240,300,380],[-238,301,380],[-237,302,379],[-235,303,380],[-233,304,380],[-231,305,380],[-230,306,380],[-228,306,379],[-226,307,380],[-225,308,380],[-223,309,380],[-221,310,380],[-220,311,380],[-218,312,380],[-216,313,379],[-215,314,379],[-213,315,380],[-211,316,379],[-210,317,380],[-208,318,380],[-206,318,380],[-205,319,379],[-203,320,379],[-201,321,380],[-200,322,380],[-198,323,380],[-197,325,380],[-195,326,380],[-193,327,379],[-192,328,380],[-190,329,380],[-189,330,380],[-187,331,379],[-186,332,380],[-184,333,380],[-182,334,380],[-181,335,380],[-179,336,380],[-178,337,379],[-176,339,380],[-175,340,380],[-173,341,380],[-172,342,380],[-170,343,380],[-169,344,380],[-167,346,380],[-166,347,380],[-164,348,380],[-163,349,380],[-161,350,380],[-159,352,380],[-158,353,380],[-156,354,380],[-155,355,380],[-154,357,380],[-152,358,380],[-151,359,380],[-149,361,380],[-148,362,380],[-146,363,380],[-145,365,379],[-143,366,380],[-142,367,380],[-140,369,380],[-139,370,380],[-137,371,380],[-136,373,380],[-134,374,380],[-133,376,380],[-131,377,380],[-130,379,380],[-128,380,380],[-127,382,380],[-126,383,380],[-124,385,380],[-123,386,380],[-121,388,380],[-120,389,380],[-118,391,380],[-117,392,380],[-115,394,380],[-114,395,380],[-113,397,380],[-111,399,380],[-110,400,380],[-108,402,380],[-107,404,380],[-105,405,380],[-104,407,380],[-103,409,379],[-101,410,380],[-100,412,380],[-98,414,380],[-97,416,380],[-95,417,380],[-94,419,380],[-92,421,380],[-91,423,380],[-90,425,380],[-88,426,380],[-87,428,380],[-85,430,380],[-84,432,380],[-82,434,380],[-81,436,380],[-80,438,380],[-78,440,380],[-77,442,380],[-75,444,379],[-74,446,380],[-72,448,380],[-71,450,380],[-69,452,380],[-68,454,380],[-67,456,380],[-65,458,380],[-64,460,380],[-62,463,380],[-61,465,380],[-59,467,380],[-58,469,380],[-56,471,380],[-55,474,380],[-53,476,380],[-52,478,379],[-51,481,380],[-49,483,380],[-48,485,380],[-46,488,380],[-45,490,380],[-43,493,380],[-42,495,380],[-40,498,380],[-39,500,380],[-37,503,380],[-36,505,380],[-34,508,380],[-33,510,380],[-31,513,380],[-30,516,380],[-28,519,380],[-27,521,380],[-25,524,380],[-24,527,380],[-22,530,380],[-21,532,380],[-19,535,380],[-17,538,380],[-16,541,379],[-14,544,380],[-13,547,380],[-11,550,380],[-10,553,380],[-8,556,380],[-6,560,380],[-5,563,380],[-3,566,380],[-2,569,380],[0,572,380],[1,576,380],[2,579,380],[4,582,380],[5,586,380],[7,589,380],[9,593,380],[10,596,380],[12,600,380],[14,604,380],[15,607,380],[17,611,380],[19,615,380],[20,618,380],[22,622,380],[24,626,380],[26,630,380],[27,634,380],[29,638,380],[31,642,380],[33,646,380],[34,651,380],[36,655,380],[38,659,380],[40,664,380],[42,668,380],[43,672,380],[45,677,380],[47,681,380],[49,686,380],[51,691,380],[53,696,380],[55,700,380],[57,705,380],[59,710,380],[60,715,380],[62,720,380],[64,726,380],[66,731,380],[68,736,380],[70,742,380],[73,747,380],[75,753,380],[77,758,380],[79,764,380],[81,770,380],[83,776,380],[85,782,380],[87,788,380],[90,794,380],[92,800,380],[94,806,380],[96,813,380],[98,819,380],[101,826,380],[103,833,380],[105,840,380],[108,847,380],[110,854,380],[113,861,380],[115,868,380],[118,876,380],[120,883,380],[123,891,380],[125,899,380],[128,907,380],[130,915,380],[133,923,380],[136,932,380],[138,940,380],[141,949,380],[144,958,380],[147,967,380],[150,976,380],[153,985,380],[155,995,380],[158,1005,380],[162,1015,380],[165,1025,380],[168,1035,380],[171,1046,380],[174,1056,379],[177,1067,380],[181,1079,379],[184,1090,379],[187,1102,379],[191,1114,380],[194,1126,379],[198,1138,379],[202,1151,380],[205,1164,379],[209,1177,380],[213,1191,380],[217,1205,380],[221,1219,379],[225,1233,380],[229,1248,380],[234,1264,380],[238,1279,380],[242,1295,379],[247,1312,380],[251,1328,380],[256,1346,379],[261,1363,379],[266,1382,379],[271,1400,379],[276,1420,379],[281,1439,380],[287,1460,379],[292,1480,379]

    ];

    var testNodes2 = [

        [-209,-1167,290],[-205,-1150,290],[-201,-1134,290],[-198,-1119,290],[-194,-1103,290],[-190,-1089,290],[-186,-1074,290],[-183,-1060,290],[-179,-1046,290],[-176,-1033,290],[-173,-1020,290],[-169,-1007,290],[-166,-995,290],[-163,-983,290],[-160,-971,290],[-157,-959,290],[-154,-948,290],[-151,-937,290],[-148,-926,290],[-146,-916,290],[-143,-906,290],[-140,-895,290],[-138,-886,290],[-135,-876,290],[-132,-867,290],[-130,-857,290],[-127,-848,290],[-125,-839,290],[-123,-831,290],[-120,-822,290],[-118,-814,290],[-116,-806,290],[-113,-798,290],[-111,-790,290],[-109,-782,290],[-107,-775,290],[-105,-767,290],[-102,-760,290],[-100,-753,290],[-98,-746,290],[-96,-739,290],[-94,-732,290],[-92,-726,290],[-90,-719,290],[-88,-713,290],[-86,-707,290],[-85,-700,290],[-83,-694,290],[-81,-688,290],[-79,-682,290],[-77,-677,290],[-75,-671,290],[-74,-665,290],[-72,-660,290],[-70,-654,290],[-68,-649,290],[-67,-644,290],[-65,-639,290],[-63,-634,290],[-62,-629,290],[-60,-624,290],[-58,-619,290],[-57,-614,290],[-55,-609,290],[-54,-605,290],[-52,-600,290],[-51,-596,290],[-49,-591,290],[-48,-587,290],[-46,-583,290],[-44,-578,290],[-43,-574,290],[-42,-570,290],[-40,-566,290],[-39,-562,290],[-37,-558,289],[-36,-554,289],[-34,-550,289],[-33,-547,290],[-31,-543,289],[-30,-539,290],[-29,-535,290],[-27,-532,290],[-26,-528,289],[-25,-525,290],[-23,-521,289],[-22,-518,290],[-21,-515,290],[-19,-511,289],[-18,-508,289],[-17,-505,289],[-15,-501,290],[-14,-498,290],[-13,-495,290],[-11,-492,290],[-10,-489,290],[-9,-486,289],[-8,-483,290],[-6,-480,290],[-5,-477,290],[-4,-474,290],[-3,-471,290],[-1,-469,290],[0,-466,290],[0,-463,289],[1,-460,290],[3,-458,290],[4,-455,290],[5,-452,290],[6,-450,290],[7,-447,290],[9,-445,290],[10,-442,290],[11,-440,290],[12,-437,290],[13,-435,290],[15,-433,290],[16,-430,290],[17,-428,290],[18,-425,289],[19,-423,290],[20,-421,290],[22,-419,290],[23,-416,290],[24,-414,290],[25,-412,290],[26,-410,290],[27,-408,290],[28,-406,289],[30,-404,290],[31,-401,290],[32,-399,290],[33,-397,290],[34,-395,290],[35,-393,290],[36,-391,290],[37,-389,290],[39,-388,290],[40,-386,290],[41,-384,290],[42,-382,290],[43,-380,290],[44,-378,290],[45,-376,290],[46,-375,290],[47,-373,290],[48,-371,290],[50,-369,290],[51,-367,290],[52,-366,290],[53,-364,290],[54,-362,290],[55,-361,290],[56,-359,290],[57,-357,290],[58,-356,290],[59,-354,290],[60,-353,290],[62,-351,290],[63,-349,290],[64,-348,290],[65,-346,290],[66,-345,290],[67,-343,290],[68,-342,290],[69,-340,290],[70,-339,290],[71,-337,290],[72,-336,290],[74,-334,290],[75,-333,290],[76,-332,290],[77,-330,290],[78,-329,290],[79,-327,290],[80,-326,290],[81,-325,290],[82,-323,290],[83,-322,290],[84,-321,290],[85,-319,290],[87,-318,290],[88,-317,290],[89,-316,290],[90,-314,290],[91,-313,290],[92,-312,290],[93,-311,290],[94,-309,290],[95,-308,290],[96,-307,290],[97,-306,290],[99,-305,290],[100,-303,290],[101,-302,290],[102,-301,290],[103,-300,290],[104,-299,290],[105,-298,290],[106,-297,290],[107,-295,290],[108,-294,290],[110,-293,290],[111,-292,290],[112,-291,290],[113,-290,290],[114,-289,290],[115,-288,290],[116,-287,290],[117,-286,290],[119,-285,290],[120,-284,290],[121,-283,290],[122,-282,290],[123,-281,290],[124,-280,290],[125,-279,290],[126,-278,290],[128,-277,290],[129,-276,290],[130,-275,290],[131,-274,290],[132,-273,290],[133,-272,290],[135,-271,290],[136,-271,290],[137,-270,290],[138,-269,290],[139,-268,290],[140,-267,290],[142,-266,290],[143,-265,290],[144,-264,290],[145,-264,290],[146,-263,290],[147,-262,290],[149,-261,290],[150,-260,290],[151,-259,290],[152,-259,290],[153,-258,290],[155,-257,290],[156,-256,290],[157,-255,290],[158,-255,290],[160,-254,290],[161,-253,290],[162,-252,290],[163,-252,290],[165,-251,290],[166,-250,290],[167,-249,290],[168,-249,290],[170,-248,290],[171,-247,290],[172,-247,290],[173,-246,290],[175,-245,290],[176,-244,290],[177,-244,290],[179,-243,290],[180,-242,290],[181,-242,290],[182,-241,290],[184,-240,290],[185,-240,290],[186,-239,290],[188,-239,290],[189,-238,290],[190,-237,290],[192,-237,290],[193,-236,290],[195,-235,290],[196,-235,290],[197,-234,290],[199,-234,290],[200,-233,290],[201,-233,290],[203,-232,290],[204,-231,290],[206,-231,290],[207,-230,290],[209,-230,290],[210,-229,290],[211,-229,290],[213,-228,290],[214,-228,290],[216,-227,290],[217,-227,290],[219,-226,290],[220,-225,290],[222,-225,290],[223,-224,290],[225,-224,290],[226,-224,290],[228,-223,290],[229,-223,290],[231,-222,290],[232,-222,290],[234,-221,290],[235,-221,290],[237,-220,290],[239,-220,290],[240,-219,290],[242,-219,290],[243,-219,290],[245,-218,290],[247,-218,290],[248,-217,290],[250,-217,290],[252,-216,290],[253,-216,290],[255,-216,290],[257,-215,290],[258,-215,290],[260,-215,290],[262,-214,290],[263,-214,290],[265,-213,290],[267,-213,290],[269,-213,290],[270,-212,290],[272,-212,290],[274,-212,290],[276,-211,290],[278,-211,290],[279,-211,290],[281,-210,290],[283,-210,290],[285,-210,290],[287,-210,290],[289,-209,290],[291,-209,290],[293,-209,290],[295,-208,290],[296,-208,290],[298,-208,290],[300,-208,290],[302,-207,290],[304,-207,290],[306,-207,290],[308,-207,290],[311,-207,290],[313,-206,290],[315,-206,290],[317,-206,290],[319,-206,290],[321,-205,290],[323,-205,290],[325,-205,290],[327,-205,290],[330,-205,290],[332,-205,290],[334,-204,290],[336,-204,290],[339,-204,290],[341,-204,290],[343,-204,290],[346,-204,290],[348,-204,290],[350,-204,290],[353,-203,290],[355,-203,290],[357,-203,290],[360,-203,290],[362,-203,290],[365,-203,290],[367,-203,290],[370,-203,290],[372,-203,290],[375,-203,290],[378,-203,290],[380,-203,290],[383,-203,290],[386,-203,290],[388,-203,290],[391,-203,289],[394,-203,290],[397,-203,290],[399,-203,290],[402,-203,290],[405,-203,289],[408,-203,290],[411,-203,290],[414,-203,290],[417,-203,290],[420,-203,289],[423,-204,290],[426,-204,290],[429,-204,290],[432,-204,290],[435,-204,290],[439,-204,290],[442,-204,290],[445,-205,290],[448,-205,290],[452,-205,290],[455,-205,290],[459,-205,290],[462,-206,290],[466,-206,290],[469,-206,290],[473,-206,290],[476,-207,290],[480,-207,290],[484,-207,290],[488,-208,290],[491,-208,290],[495,-208,290],[499,-209,290],[503,-209,290],[507,-209,290],[511,-210,290],[515,-210,290],[519,-211,290],[524,-211,290],[528,-212,290],[532,-212,290],[537,-212,290],[541,-213,290],[545,-213,290],[550,-214,290],[555,-215,290],[559,-215,290],[564,-216,290],[569,-216,290],[574,-217,290],[579,-218,290],[584,-218,290],[589,-219,290],[594,-220,290],[599,-220,290],[604,-221,290],[610,-222,290],[615,-223,290],[621,-223,290],[627,-224,290],[632,-225,290],[638,-226,290],[644,-227,290],[650,-228,290],[656,-229,290],[662,-230,290],[669,-231,290],[675,-232,290],[682,-233,290],[688,-234,290],[695,-235,290],[702,-236,290],[709,-237,290],[716,-238,290],[723,-240,290],[731,-241,290],[738,-242,290],[746,-243,290],[753,-245,290],[761,-246,290],[769,-248,290],[778,-249,290],[786,-251,290],[795,-252,290],[803,-254,290],[812,-255,290],[821,-257,290],[831,-259,290],[840,-261,290],[850,-263,290],[860,-264,290],[870,-266,290],[880,-268,290],[890,-270,290],[901,-273,290],[912,-275,290],[923,-277,290],[935,-279,290],[947,-282,290],[959,-284,290],[971,-287,290],[984,-289,290],[997,-292,290],[1010,-295,290],[1024,-298,290],[1038,-300,290],[1052,-303,290],[1067,-307,289],[1082,-310,290],[1097,-313,290],[1113,-317,290],[1129,-320,290]


    ];

    var testNodes3 = [

[-283,-1309,340],[-277,-1290,340],[-272,-1272,340],[-267,-1255,340],[-262,-1237,340],[-258,-1221,340],[-253,-1205,340],[-248,-1189,340],[-244,-1173,340],[-240,-1158,340],[-235,-1144,340],[-231,-1129,340],[-227,-1115,340],[-223,-1102,340],[-219,-1089,340],[-215,-1076,340],[-211,-1063,340],[-208,-1051,340],[-204,-1038,340],[-200,-1027,340],[-197,-1015,340],[-193,-1004,340],[-190,-993,340],[-187,-982,340],[-183,-971,340],[-180,-961,340],[-177,-951,340],[-174,-941,340],[-171,-931,340],[-168,-921,340],[-165,-912,340],[-162,-903,340],[-159,-894,340],[-156,-885,340],[-153,-876,340],[-150,-868,340],[-148,-860,340],[-145,-851,340],[-142,-843,340],[-140,-835,340],[-137,-828,340],[-135,-820,340],[-132,-813,340],[-130,-805,340],[-127,-798,339],[-125,-791,340],[-122,-784,339],[-120,-777,339],[-118,-771,340],[-115,-764,339],[-113,-757,339],[-111,-751,339],[-109,-745,340],[-106,-739,340],[-104,-732,340],[-102,-726,339],[-100,-721,340],[-98,-715,339],[-96,-709,340],[-94,-703,339],[-92,-698,339],[-89,-692,340],[-87,-687,340],[-85,-682,339],[-83,-676,340],[-82,-671,339],[-80,-666,340],[-78,-661,339],[-76,-656,340],[-74,-651,339],[-72,-647,340],[-70,-642,340],[-68,-637,340],[-66,-633,339],[-65,-628,340],[-63,-624,340],[-61,-619,339],[-59,-615,340],[-57,-611,340],[-56,-607,339],[-54,-602,340],[-52,-598,340],[-51,-594,339],[-49,-590,340],[-47,-586,339],[-45,-582,339],[-44,-578,340],[-42,-575,339],[-40,-571,340],[-39,-567,340],[-37,-563,340],[-36,-560,340],[-34,-556,340],[-32,-553,340],[-31,-549,340],[-29,-546,340],[-28,-542,340],[-26,-539,340],[-24,-536,340],[-23,-532,340],[-21,-529,340],[-20,-526,340],[-18,-523,340],[-17,-520,340],[-15,-517,340],[-14,-513,340],[-12,-510,340],[-11,-507,340],[-9,-504,340],[-8,-502,340],[-6,-499,340],[-5,-496,340],[-3,-493,340],[-2,-490,340],[-1,-487,340],[0,-485,340],[1,-482,340],[3,-479,339],[4,-477,340],[6,-474,340],[7,-471,340],[8,-469,339],[10,-466,340],[11,-464,340],[13,-461,340],[14,-459,340],[15,-456,340],[17,-454,340],[18,-451,340],[20,-449,340],[21,-447,340],[22,-444,340],[24,-442,340],[25,-440,340],[26,-437,340],[28,-435,340],[29,-433,340],[31,-431,340],[32,-429,340],[33,-427,340],[35,-424,340],[36,-422,340],[37,-420,340],[39,-418,340],[40,-416,340],[41,-414,340],[43,-412,340],[44,-410,340],[45,-408,340],[47,-406,340],[48,-404,340],[49,-402,340],[50,-400,340],[52,-398,340],[53,-397,339],[54,-395,340],[56,-393,339],[57,-391,340],[58,-389,340],[60,-387,340],[61,-386,340],[62,-384,340],[64,-382,340],[65,-380,340],[66,-379,340],[67,-377,340],[69,-375,340],[70,-374,340],[71,-372,340],[73,-370,340],[74,-369,340],[75,-367,340],[77,-366,340],[78,-364,340],[79,-362,339],[80,-361,340],[82,-359,340],[83,-358,340],[84,-356,340],[86,-355,340],[87,-353,340],[88,-352,340],[89,-350,340],[91,-349,340],[92,-347,340],[93,-346,340],[95,-344,340],[96,-343,340],[97,-342,340],[99,-340,340],[100,-339,340],[101,-338,340],[102,-336,340],[104,-335,340],[105,-333,340],[106,-332,340],[108,-331,340],[109,-330,340],[110,-328,340],[112,-327,340],[113,-326,340],[114,-324,340],[116,-323,340],[117,-322,340],[118,-321,340],[120,-319,340],[121,-318,340],[122,-317,340],[124,-316,340],[125,-315,340],[126,-313,340],[128,-312,340],[129,-311,340],[130,-310,340],[132,-309,340],[133,-308,340],[134,-307,340],[136,-305,340],[137,-304,340],[138,-303,340],[140,-302,340],[141,-301,340],[142,-300,340],[144,-299,340],[145,-298,340],[146,-297,340],[148,-296,340],[149,-295,340],[151,-294,340],[152,-293,340],[153,-292,340],[155,-291,340],[156,-290,340],[157,-289,340],[159,-288,340],[160,-287,340],[162,-286,340],[163,-285,340],[165,-284,340],[166,-283,340],[167,-282,340],[169,-281,340],[170,-280,340],[172,-279,340],[173,-278,340],[175,-277,340],[176,-277,340],[177,-276,340],[179,-275,340],[180,-274,340],[182,-273,340],[183,-272,340],[185,-271,340],[186,-271,340],[188,-270,340],[189,-269,340],[191,-268,340],[192,-267,340],[194,-266,340],[195,-266,340],[197,-265,340],[198,-264,340],[200,-263,340],[201,-262,340],[203,-262,340],[204,-261,340],[206,-260,340],[208,-259,340],[209,-259,340],[211,-258,340],[212,-257,340],[214,-256,340],[215,-256,340],[217,-255,340],[219,-254,340],[220,-254,340],[222,-253,340],[223,-252,340],[225,-251,340],[227,-251,340],[228,-250,340],[230,-249,340],[232,-249,340],[233,-248,340],[235,-247,340],[237,-247,340],[238,-246,340],[240,-246,340],[242,-245,340],[243,-244,340],[245,-244,340],[247,-243,340],[249,-242,340],[250,-242,340],[252,-241,340],[254,-241,340],[256,-240,340],[258,-239,340],[259,-239,340],[261,-238,340],[263,-238,340],[265,-237,340],[267,-237,340],[268,-236,340],[270,-236,340],[272,-235,340],[274,-234,340],[276,-234,340],[278,-233,340],[280,-233,340],[282,-232,340],[284,-232,340],[285,-231,340],[287,-231,340],[289,-230,340],[291,-230,340],[293,-229,340],[295,-229,340],[297,-229,340],[299,-228,340],[301,-228,340],[303,-227,340],[306,-227,340],[308,-226,340],[310,-226,339],[312,-225,340],[314,-225,340],[316,-225,340],[318,-224,339],[320,-224,340],[322,-223,340],[325,-223,340],[327,-223,340],[329,-222,340],[331,-222,339],[334,-222,340],[336,-221,339],[338,-221,340],[340,-221,340],[343,-220,340],[345,-220,340],[347,-220,340],[350,-219,339],[352,-219,339],[355,-219,340],[357,-218,340],[359,-218,340],[362,-218,340],[364,-217,340],[367,-217,340],[369,-217,340],[372,-217,340],[374,-216,340],[377,-216,340],[380,-216,340],[382,-216,340],[385,-215,340],[387,-215,340],[390,-215,339],[393,-215,339],[396,-215,340],[398,-214,340],[401,-214,340],[404,-214,340],[407,-214,340],[410,-214,339],[412,-213,340],[415,-213,340],[418,-213,340],[421,-213,340],[424,-213,340],[427,-213,340],[430,-213,340],[433,-213,340],[436,-212,340],[439,-212,340],[443,-212,340],[446,-212,340],[449,-212,340],[452,-212,340],[455,-212,340],[459,-212,340],[462,-212,340],[465,-212,340],[469,-212,340],[472,-212,340],[476,-212,340],[479,-212,340],[483,-212,340],[486,-212,340],[490,-212,340],[494,-212,340],[497,-212,340],[501,-212,340],[505,-212,340],[509,-212,340],[512,-213,340],[516,-213,340],[520,-213,340],[524,-213,340],[528,-213,340],[532,-213,340],[536,-213,340],[541,-214,340],[545,-214,340],[549,-214,340],[553,-214,340],[558,-214,340],[562,-215,340],[567,-215,340],[571,-215,340],[576,-215,340],[580,-216,340],[585,-216,340],[590,-216,340],[595,-217,340],[600,-217,340],[604,-217,340],[609,-218,340],[615,-218,340],[620,-219,340],[625,-219,340],[630,-219,340],[636,-220,340],[641,-220,340],[647,-221,340],[652,-221,340],[658,-222,340],[664,-223,340],[669,-223,340],[675,-224,340],[681,-224,340],[687,-225,340],[694,-226,340],[700,-226,340],[706,-227,340],[713,-228,340],[719,-228,340],[726,-229,340],[733,-230,340],[740,-231,340],[747,-232,340],[754,-232,340],[761,-233,340],[768,-234,340],[776,-235,340],[783,-236,340],[791,-237,340],[799,-238,340],[807,-239,340],[815,-240,340],[823,-241,340],[832,-242,340],[840,-244,340],[849,-245,340],[858,-246,340],[867,-247,340],[876,-249,340],[886,-250,340],[895,-251,340],[905,-253,340],[915,-254,340],[925,-256,340],[935,-257,340],[946,-259,340],[957,-261,340],[968,-262,340],[979,-264,340],[991,-266,340],[1002,-268,340],[1014,-270,340],[1027,-272,339],[1039,-274,340],[1052,-276,340],[1065,-278,340],[1079,-280,340],[1093,-283,340],[1107,-285,340],[1121,-287,340],[1136,-290,340],[1151,-293,340],[1167,-295,340],[1183,-298,340],[1199,-301,340],[1216,-304,340],[1233,-307,340],[1251,-310,340],[1269,-313,340],[1287,-317,340]

    ];


    //
    // var testNodes4 = [
    // ];
    //
    //
    // var testNodes5 = [
    // ];
    //
    //
    // var testNodes6 = [
    // ];

    var circleSize = 5;
    create_cloud_point(testNodes1, ColorsTable[0], circleSize, 0.5);

    create_cloud_point(testNodes2, ColorsTable[3], circleSize, 0.5);

    create_cloud_point(testNodes3, ColorsTable[7], circleSize, 0.5);

    // create_cloud_point(testNodes4, ColorsTable[0], circleSize, 0.3);
    //
    // create_cloud_point(testNodes5, ColorsTable[3], circleSize, 0.3);
    //
    // create_cloud_point(testNodes6, ColorsTable[7], circleSize, 0.3);


    circleSize = 15;
    var node = [357,466,380]; // AoA anchor 1
    //var node = [0,0,0]; // AoA anchor 1
    create_test_Sphere(node, ColorsTable[10], circleSize, 0.9);

    circleSize = 15;
    var node = [365,-489,290]; // AoA anchor 2
    //var node = [955,0,0]; // AoA anchor 2
    create_test_Sphere(node, ColorsTable[6], circleSize, 0.9);

    circleSize = 15;
    var node = [-360,-453,340]; // AoA anchor 3
    //var node = [912,-724,0]; // AoA anchor 3
    create_test_Sphere(node, ColorsTable[1], circleSize, 0.9);

    circleSize = 15;
    //var node = [271,521,300]; // AoA 1
    var node = [317,457,379]; // AoA 1
    create_test_Sphere(node, ColorsTable[0], circleSize, 0.9);

    circleSize = 15;
    //var node = [444,-418,300]; // AoA 2
    var node = [325,-497,289]; // AoA 2
    create_test_Sphere(node, ColorsTable[3], circleSize, 0.9);

    circleSize = 15;
    //var node = [-276,-509,300]; // AoA 3
    var node = [-399,-461,339]; // AoA 3
    create_test_Sphere(node, ColorsTable[7], circleSize, 0.9);

    return;
}

    function create_test_Sphere(node, nodecolor, circleSize, circleOpacity)
    {
        var material = new THREE.MeshBasicMaterial( {color: nodecolor, opacity: circleOpacity} );
        material.transparent = true;
        var sphereFT = new THREE.Mesh( new THREE.SphereGeometry( circleSize, 10, 10 ), material );
        sphereFT.position.set( node[0], node[1], node[2] );
        sphereFT.visible = true;
        scene.add( sphereFT );

        return sphereFT;
    }
