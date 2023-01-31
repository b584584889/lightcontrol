/*
 * Author: ikki Chung
 * Date: 21 Mar 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var upload_img_width = 0;
var upload_img_height = 0;

var _URL = window.URL || window.webkitURL;

function get_maplayerarea(maplayerid, callback)
{
    var targetURL = gl_target_server+ "/php/map.php";

    $.getJSON(targetURL, 'loadmaplayerarea=1&project_id='+project_id+'&maplayerid='+maplayerid, function(data)
    {
        callback(data);
    });
}

function update_orig_node_info(maplayerid, posX, posY)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['updateorig'] = '1';
    jsondata['maplayerid'] = maplayerid;
    jsondata['POSX'] = ''+posX;
    jsondata['POSY'] = ''+posY;

    var targetURL = gl_target_server + "/php/map.php";

    jQuery.ajax({
        url: targetURL,
        type: 'POST',
        data: jsondata,
        dataType: 'json',
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
    return false;
}

function update_map_info(maplayerid, map_scale, map_coeff)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['updatemap'] = '1';
    jsondata['maplayerid'] = maplayerid;
    jsondata['SCALE'] = ''+map_scale;
    jsondata['COEFF'] = ''+map_coeff;

    var targetURL = gl_target_server + "/php/map.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      dataType: 'json',
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
   return false;
}

function update_map_node_info(maplayerid, type, macaddress, nodeid, posX, posY, posZ, lat, lng)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['EDIT_NODE'] = 1;
    jsondata['maplayerid'] = maplayerid;
    jsondata['TYPE'] = type;
    jsondata['MAC'] = macaddress;
    jsondata['NODEID'] = nodeid;
    jsondata['POSX'] = posX;
    jsondata['POSY'] = posY;
    jsondata['POSZ'] = posZ;
    jsondata['LAT'] = lat;
    jsondata['LNG'] = lng;

    // alert('update_map_node_info() '+type+' nodeid:'+nodeid+"("+ posX +","+ posY +","+ posZ +")");

    Node_Updateing = true;
    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
    url: targetURL,
    type: 'POST',
    data: jsondata,
    success:
    function(response)
    {
        if ( response.status == 400 )
        {
            alert("update Error! response = " + JSON.stringify(response));
        }
        else
        {
            //alert("update success! response = " + JSON.stringify(response));
            //var pos_id = gl_updating_element_id+"_POS";
            //var position = '('+jsondata['POSX']+','+jsondata['POSY']+','+jsondata['POSZ']+')';
            //document.getElementById(pos_id).innerText = position;
        }
    },
    error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    })
    .complete(function() { Node_Updateing = false; });
    return false;
}

function update_maplayer_info(maplayerid, data_key, data_value)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['updatemaplayerinfo'] = '1';
    jsondata['maplayerid'] = maplayerid;
    jsondata['data_key'] = data_key;
    jsondata['data_value'] = data_value;

    var targetURL = gl_target_server + "/php/map.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      dataType: 'json',
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
   return false;
}

//
// create new map layer
//
function create_maplayer(layer_name, callback, show_index)
{
 	var json = {};
    json['project_id'] = ''+project_id;
 	json['createmaplayer'] = 1;
    json['layer_name'] = layer_name;

    var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       data: json,
       success:
         function(response)
         {
             callback(show_index);
         },
       error:
       	function(xhr, status, error)
       	{
 			alert(error);
 		    alert(xhr.responseText);
 		}
    });
}

function delete_maplayer(maplayerid, callback)
{
    var json = {};
    json['project_id'] = ''+project_id;
 	json['deletemaplayer'] = 1;
    json['maplayerid'] = maplayerid;

    var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       data: json,
       success:
         function(response)
         {
             callback();
         },
       error:
       	function(xhr, status, error)
       	{
 			alert(error);
 		    alert(xhr.responseText);
 		}
    });
}

//
// create new map area
//
function create_maplayerarea(orig_this, maplayerid, map_shape, area_id, area_name, area_type, area_index, success_callback)
{
 	var json = {};
    json['project_id'] = ''+project_id;
 	//json['NEW_AREA'] = 1;
    json['createmaplayerarea'] = 1;
    json['maplayerid'] = maplayerid;
    json['geojson'] = map_shape;
 	json['areaid'] = area_id;
    json['areaname'] = area_name;
    json['type'] = area_type;

    var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       data: json,
       success:
         function(response)
         {
           success_callback(orig_this, response, area_index);
         },
       error:
       	function(xhr, status, error)
       	{
 			alert(error);
 		    alert(xhr.responseText);
 		}
    });
}

//
// update a map area
//
//function update_map_area_shape(area_id, map_shape, area_name)
function update_maplayerarea_shape(maplayerid, area_id, map_shape, area_name)
{
     //alert("update_map_area() map_shape = " + JSON.stringify(map_shape));

     var json = {};
     json['project_id'] = ''+project_id;
  	 //json['EDIT_AREA'] = 1;
     json['updatemaplayerarea'] = 1;
     json['maplayerid'] = maplayerid;
     json['geojson'] = map_shape;
  	 json['areaid'] = area_id;
     json['areaname'] = area_name;

     var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
        url: targetURL,
        type: 'POST',
        //contentType: "application/json",
        dataType: "json",
        data: json,
       success:
         function(result)
         {
             //alert("update_map_area success!result="+result);
         },
       error:
         function(xhr, status, error)
         {
             //alert(error);
             alert("error="+error+",responseText="+xhr.responseText);
         }
     });
}

//function update_map_area(area_id, map_shape, area_name, area_type)
function update_maplayerarea(maplayerid, area_id, map_shape, area_name, area_type)
{
 //alert("update_map_area() map_shape = " + JSON.stringify(map_shape));

     var json = {};
     json['project_id'] = ''+project_id;
     //json['EDIT_AREA'] = 1;
     json['updatemaplayerarea'] = 1;
     json['maplayerid'] = maplayerid;
     json['geojson'] = map_shape;
     json['areaid'] = area_id;
     json['areaname'] = area_name;
     json['type'] = area_type;

     var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
        url: targetURL,
        type: 'POST',
        //contentType: "application/json",
        dataType: "json",
        data: json,
       success:
         function(result)
         {
             //alert("update_map_area success!result="+result);
         },
       error:
         function(xhr, status, error)
         {
             //alert(error);
             alert("error="+error+",responseText="+xhr.responseText);
         }
     });
}

//
// delete a map area
//
//function delete_map_area_from_center(area_id)
function delete_maplayerarea(maplayerid, area_id)
{
     var json = {};
     json['project_id'] = ''+project_id;
  	 //json['DELETE_AREA'] = 1;
     json['deletemaplayerarea'] = 1;
     json['maplayerid'] = maplayerid;
     json['areaid'] = area_id;

     var targetURL = gl_target_server + "/php/map.php";


     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       //contentType: "application/json",
       dataType: "json",
       data: json,
       success:
         function(result)
         {
             //alert("delete_map_area success, area_id="+area_id);
         },
       error:
         function(xhr, status, error)
         {
             alert(error);
             alert(xhr.responseText);
         }
     });
}

// $("#imageUpload").change(
// function()
// {
//
//      var file=this.files[0];
//      var imageFile=file.type;
//      var match=["image/jpeg","image/png","image/jpg"];
//
//      if(!((imageFile==match[0]) || (imageFile==match[1]) || (imageFile==match[2])))
//      {
//          $("#upload_msg").text("Only Jpeg/ Jpg /Png /Gif are allowed");
//          return false;
//      }
//      else
//      {
//          img = new Image();
//          img.onload = function ()
//          {
//              upload_img_width = this.width;
//              upload_img_height = this.height;
//              //alert("upload_img_width:" +this.width + " upload_img_height:" + this.height);
//          };
//          img.src = _URL.createObjectURL(file);
//
//          $("#upload_msg").hide();
//      }
// });

function submit_newmap(form_this, ui_index, layerid)
{
    //console.log("submit() ui_index:"+ui_index);

    var mapFile = document.getElementById("imageUpload_"+ui_index).files[0];
    if ( mapFile == undefined )
    {
        alert('Please assign image file!');
        $("#upload_msg_"+ui_index).text("Please assign image file!");
        return false;
    }

    var imageFile = mapFile.type;
    var match=["image/jpeg","image/png","image/jpg"];

    if(!((imageFile==match[0]) || (imageFile==match[1]) || (imageFile==match[2])))
    {
        $("#upload_msg_"+ui_index).text("Only Jpeg/ Jpg /Png /Gif are allowed!");
        //alert('Only Jpeg/ Jpg /Png /Gif are allowed!');
        return false;
    }
    else
    {
        img = new Image();
        img.onload = function ()
        {
            upload_image(form_this, ui_index, layerid, mapFile, this.width, this.height);
            return false;
        };
        img.src = _URL.createObjectURL(mapFile);
    }
    return false;
}

function upload_image(form_this, ui_index, layerid, mapFile, upload_img_width, upload_img_height)
{
    //alert('upload_image() mapFile.name:'+mapFile.name+" upload_img_width:"+upload_img_width+" upload_img_height:"+upload_img_height);

    //var formdata = new FormData($("#formContent_"+ui_index)[0]);
    var formdata = new FormData(form_this);
    formdata.append('upload_img_width', upload_img_width);
    formdata.append('upload_img_height', upload_img_height);
    formdata.append('project_id', ''+project_id);
    formdata.append('maplayerid', ''+layerid);

    $("#uploadIm_"+ui_index).show();

    var targetURL = gl_target_server + "/php/uploadmap.php";

    jQuery.ajax({
        url: targetURL,
        type: 'POST',
        processData: false,
        contentType: false,
        data: formdata,
        success:
        function(response)
        {
            $("#uploadIm_"+ui_index).hide();
            $("#upload_msg_"+ui_index).show().html("Image uploaded successfully<br>");

            //
            // change map
            //
            if (window.glLiveMapList[ui_index])
            {
                //var mapFile = document.getElementById("imageUpload_"+ui_index).files[0];
                var usr_map_url = '/upload/' + mapFile.name;
                window.glLiveMapList[ui_index].changeUserMap(usr_map_url, upload_img_width, upload_img_height);
            }
            else
            {
                $("#upload_msg_"+ui_index).show().html("Can not change map.<br>");
            }

        },
        error:
        function(xhr, status, error)
        {
            $("#uploadIm_"+ui_index).hide();
            alert("upload image error!");
        }
    });

    return false;
}

// $("#formContent").submit(function(e)
// {
//      e.preventDefault();
//
//      var mapFile = document.getElementById("imageUpload").files[0];
//      if ( mapFile == undefined )
//      {
//          alert('Please assign image file!');
//          return false;
//      }
//
//      var formdata = new FormData($("#formContent")[0]);
//
//      //var formdata = new FormData(this);
//      formdata.append('upload_img_width', upload_img_width);
//      formdata.append('upload_img_height', upload_img_height);
//      formdata.append('project_id', ''+project_id);
//
//      $("#uploadIm").show();
//
//      var targetURL = gl_target_server + "/php/uploadmap.php";
//      //alert("targetURL=" + targetURL);
//
//      jQuery.ajax({
//        url: targetURL,
//        type: 'POST',
//        processData: false,
//        contentType: false,
//        data: formdata,
//        success:
//            function(response)
//            {
//                //alert("upload done! response = " + response);
//                $("#uploadIm").hide();
//                $("#upload_msg").show().html("Image uploaded successfully<br>");
//
//                 //
//                 // change map
//                 //
//                 if (window.glLiveMap)
//                 {
//                     var mapFile = document.getElementById("imageUpload").files[0];
//                     var usr_map_url = '/upload/' + mapFile.name;
//                     window.glLiveMap.changeUserMap(usr_map_url, upload_img_width, upload_img_height);
//                 }
//                 else
//                 {
//                     $("#upload_msg").show().html("Can not change map.<br>");
//                 }
//
//           },
//        error:
//            function(xhr, status, error)
//            {
//                //alert("error = " + error);
//                //alert("xhr.responseText = " + xhr.responseText);
//                $("#uploadIm").hide();
//                alert("upload image error!");
//            }
//      });
//
//      return false;
//
// });

function area_type_select(area_type)
{
     var select_value = $("#area_type_"+area_type).html();
     var select_id = $("#area_type_"+area_type).attr("data-id");

     $("#area_type_show").html(select_value);
     $("#area_type_show").attr("data-id", select_id);
}


function convert_area_type_name(areatype)
{
     if (areatype == "0")
     {
         return "普通";
     }
     else
     if (areatype == "1")
     {
         return "電子圍欄";
     }
     else
     if (areatype == "2")
     {
         return "電子點名";
     }
     else
     if (areatype == "3")
     {
         return "房間";
     }
     else
     if (areatype == "4")
     {
         return "教室";
     }
     else
     if (areatype == "5")
     {
         return "病房";
     }
     else
     if (areatype == "6")
     {
         return "病床";
     }
     else
     if (areatype == "999")
     {
         return "虛擬牆";
     }
     else
     {
         return "普通";
     }
}

function getRandomInt(min, max)
{
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

//浮點數相加
function FloatAdd(arg1, arg2)
{
    var r1, r2, m;
    try { r1 = arg1.toString().split(".")[1].length; } catch (e) { r1 = 0; }
    try { r2 = arg2.toString().split(".")[1].length; } catch (e) { r2 = 0; }
    m = Math.pow(10, Math.max(r1, r2));
    return (FloatMul(arg1, m) + FloatMul(arg2, m)) / m;
}

//浮點數相減
function FloatSubtraction(arg1, arg2)
{
    var r1, r2, m, n;
    try { r1 = arg1.toString().split(".")[1].length } catch (e) { r1 = 0 }
    try { r2 = arg2.toString().split(".")[1].length } catch (e) { r2 = 0 }
    m = Math.pow(10, Math.max(r1, r2));
    n = (r1 >= r2) ? r1 : r2;
    return ((arg1 * m - arg2 * m) / m).toFixed(n);
}

//浮點數相乘
function FloatMul(arg1, arg2)
{
    var m = 0, s1 = arg1.toString(), s2 = arg2.toString();
    try { m += s1.split(".")[1].length; } catch (e) { }
    try { m += s2.split(".")[1].length; } catch (e) { }
    return Number(s1.replace(".", "")) * Number(s2.replace(".", "")) / Math.pow(10, m);
}

//浮點數相除
function FloatDiv(arg1, arg2)
{
    var t1 = 0, t2 = 0, r1, r2;
    try { t1 = arg1.toString().split(".")[1].length } catch (e) { }
    try { t2 = arg2.toString().split(".")[1].length } catch (e) { }
    r1 = Number(arg1.toString().replace(".", ""));
    r2 = Number(arg2.toString().replace(".", ""));
    return (r1 / r2) * Math.pow(10, t2 - t1);
}

function rotatePoint(latlngPoint, angleDeg, latlngCenter)
{
    var angleRad = angleDeg * Math.PI / 180;

    //console.log("rotatePoint() angleDeg:"+angleDeg +", angleRad:"+angleRad);
   //console.log("rotatePoint() Math.cos(angleRad):"+Math.cos(angleRad) +", Math.sin(angleRad):"+Math.sin(angleRad));

    var lngRotate = Math.cos(angleRad)*(latlngPoint.lng - latlngCenter.lng) - Math.sin(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lng;
    var latRotate = Math.sin(angleRad)*(latlngPoint.lng - latlngCenter.lng) + Math.cos(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lat;
    return (new L.latLng(latRotate, lngRotate));
}
