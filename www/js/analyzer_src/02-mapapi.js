/*
 * Author: ikki Chung
 * Date: 21 Mar 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

 //
 // create new map area to 7688
 //
 function create_map_area_on7688(map_shape, area_id, area_index, success_callback)
 {
 	var json = {};
    json['project_id'] = ''+project_id;
    json['NEW_AREA'] = 1;
 	json['geojson'] = map_shape;
 	json['areaid'] = area_id;

    var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       //contentType: "application/json",
       //dataType: "json",
       data: json,
       success:
         function(response)
         {
           success_callback(response, area_index);
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
 function update_map_area_on7688(area_id, map_shape, area_name)
 {
     //alert("update_map_area() map_shape = " + JSON.stringify(map_shape));

     var json = {};
     json['project_id'] = ''+project_id;
     json['EDIT_AREA'] = 1;
  	 json['geojson'] = map_shape;
  	 json['areaid'] = area_id;

     var targetURL = gl_target_server + "/php/map.php";

     return jQuery.ajax({
        url: targetURL,
        type: 'POST',
        //contentType: "application/json",
        //dataType: "json",
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

 function getRandomInt(min, max)
 {
   return Math.floor(Math.random() * (max - min + 1)) + min;
 }

 function get_maparea_From7688(callback)
 {
     var targetURL = gl_target_server+ "/php/map.php";

     $.getJSON(targetURL, 'project_id='+project_id, function(data)
     {
         callback(data);
     });
 }
 //
 // delete a map area
 //
 function delete_map_area_on7688(area_id)
 {
     var json = {};
     json['project_id'] = ''+project_id;
     json['DELETE_AREA'] = 1;
  	 json['areaid'] = area_id;

     var targetURL = gl_target_server + "/php/map.php";


     return jQuery.ajax({
       url: targetURL,
       type: 'POST',
       //contentType: "application/json",
       //dataType: "json",
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


 function update_map_info(map_scale, map_coeff)
 {
     //alert("update_map_info() map_scale=" + map_scale +", map_zoom="+ map_zoom);
     //alert("update_orig_node_info() posX=" + posX +", posY="+ posY);

     var jsondata = {};
     jsondata['project_id'] = ''+project_id;
     jsondata['updatemap'] = '1';
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
             //alert("update_map_info update success!");
             //alert("response=" + response);
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

 var upload_img_width = 0;
 var upload_img_height = 0;

 var _URL = window.URL || window.webkitURL;

 $("#imageUpload").change(
 function()
 {

     var file=this.files[0];
     var imageFile=file.type;
     var match=["image/jpeg","image/png","image/jpg"];

     if(!((imageFile==match[0]) || (imageFile==match[1]) || (imageFile==match[2])))
     {
         $("#upload_msg").text("Only Jpeg/ Jpg /Png /Gif are allowed");
         return false;
     }
     else
     {
         img = new Image();
         img.onload = function ()
         {
             upload_img_width = this.width;
             upload_img_height = this.height;
             //alert("upload_img_width:" +this.width + " upload_img_height:" + this.height);
         };
         img.src = _URL.createObjectURL(file);

         $("#upload_msg").hide();
     }
 });

 $("#formContent").submit(function(e)
 {
     e.preventDefault();

     var mapFile = document.getElementById("imageUpload").files[0];
     if ( mapFile == undefined )
     {
         alert('Please assign image file!');
         return false;
     }

     var formdata = new FormData($("#formContent")[0]);

     //var formdata = new FormData(this);
     formdata.append('upload_img_width', upload_img_width);
     formdata.append('upload_img_height', upload_img_height);
     formdata.append('project_id', ''+project_id);

     $("#uploadIm").show();

     var targetURL = gl_target_server + "/php/uploadmap.php";
     //alert("targetURL=" + targetURL);

     jQuery.ajax({
       url: targetURL,
       type: 'POST',
       processData: false,
       contentType: false,
       data: formdata,
       success:
           function(response)
           {
               //alert("upload done! response = " + response);
               $("#uploadIm").hide();
               $("#upload_msg").show().html("Image uploaded successfully<br>");

                //
                // change map
                //
                if (window.glLiveMap)
                {
                    var mapFile = document.getElementById("imageUpload").files[0];
                    var usr_map_url = '/images/' + mapFile.name;
                    window.glLiveMap.changeUserMap(usr_map_url, upload_img_width, upload_img_height);
                }
                else
                {
                    $("#upload_msg").show().html("Can not change map.<br>");
                }

          },
       error:
           function(xhr, status, error)
           {
               //alert("error = " + error);
               //alert("xhr.responseText = " + xhr.responseText);
               $("#uploadIm").hide();
               alert("upload image error!");
           }
     });

     return false;

 });
