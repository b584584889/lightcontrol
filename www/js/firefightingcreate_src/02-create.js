/*
 * Author: ikki Chung
 * Date: 10 Aug 2021
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glLiveMap;
var glAddressList = [];
var glCurrentTargetAddressIndex = -1;

// start loading
check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                create_map();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function create_map()
{
    glLiveMap = new Indoor2DMap("mapid");

    glLiveMap.setMapCoeff(0, 0, 10240, 10240, 5, 100000, 121.51, 25.04, 0.0);
    glLiveMap.setMapImage("/images/empty.png", "Indoor 2D Map");
    glLiveMap.setMapLayerInfo(1);
    // glLiveMap.addOriginMarker(0, 0, 0);

    glLiveMap.showGoolgeMap();
    glLiveMap.showOriginLayer();

}

function findadressloactionbtn_click()
{
    var Address = document.getElementById("Address").value;

    glAddressList = [];

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'findlocation=1'+'&address='+Address , function(data)
    {
        $.each(data, function(index, addressdata)
        {
            glAddressList.push(addressdata);
        });

    })
    .complete(function() {
        show_addresses();
    });
}

function show_addresses()
{
    document.getElementById("Address_longitude").innerHTML = "經度:";
    document.getElementById("Address_latitude").innerHTML = "緯度:";

    glCurrentTargetAddressIndex = -1;
    glLiveMap.clearAddressTarget();

    var dropdown_addresses_ul = document.getElementById("dropdown_addresses");
    while (dropdown_addresses_ul.hasChildNodes())
    {
        dropdown_addresses_ul.removeChild(dropdown_addresses_ul.firstChild);
    }

    for	(var index = 0; index < glAddressList.length; index++)
    {
        var addressData = glAddressList[index];

        var defaultchecked = false;
        if (index == 0 )
        {
            defaultchecked = true;
            document.getElementById("cur_address").innerHTML = addressData.address + '<span class="caret"></span>';
            document.getElementById("Address_longitude").innerHTML = "經度:" + addressData.location.lng;
            document.getElementById("Address_latitude").innerHTML = "緯度:" + addressData.location.lat;

            glCurrentTargetAddressIndex = index;
            focus_map(addressData);

        }
        var item_li = create_dropdown_address_item(index, addressData.address, defaultchecked);
        dropdown_addresses_ul.appendChild(item_li);
    }
}

function create_dropdown_address_item(addressindex, address, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", addressindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_address_click(\""+ addressindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_address_"+addressindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_address_click(\""+ addressindex +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+address));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_address_click = false;
function dropdown_checkbox_address_click(addressindex)
{
    glDropdown_checkbox_address_click = true;
}

function dropdown_address_click(addressindex)
{
    var addressData = glAddressList[addressindex];
    glCurrentTargetAddressIndex = addressindex;

    focus_map(addressData);

    document.getElementById("cur_address").innerHTML = addressData.address + '<span class="caret"></span>';
    document.getElementById("Address_longitude").innerHTML = "經度:" + addressData.location.lng;
    document.getElementById("Address_latitude").innerHTML = "緯度:" + addressData.location.lat;

    glDropdown_checkbox_address_click = false;
}

function focus_map(addressData)
{
    glLiveMap.setBounds(addressData.northeast.lat, addressData.northeast.lng, addressData.southwest.lat, addressData.southwest.lng);

    glLiveMap.setTargetLocation(addressData.address, addressData.location.lat, addressData.location.lng);
}

function createNewFireFightingbtn_click()
{
     if ( glCurrentTargetAddressIndex == -1)
     {
         alert("請輸入案場地址!");
         return;
     }

     var addressData = glAddressList[glCurrentTargetAddressIndex];

     var jsondata = {};
     jsondata['project_id'] = project_id;
     jsondata['createFireFighting'] = 1;
     jsondata['address'] = addressData.address;
     jsondata['lat'] = addressData.location.lat;
     jsondata['lng'] = addressData.location.lng;

    var targetURL = gl_target_server + "/php/firefighting.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      dataType: 'json',
      success:
        function(response)
        {
            // alert("response = " + response);
            if (response.casenumber != undefined)
            {
                document.getElementById("create_msg").innerHTML = '新增案場編號:' + response.casenumber;
                alert('新增案場編號:' + response.casenumber);
            }
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

function open_firefighting_create_page()
{
    var win = window.open('/index.php/vilsfirefightingcreate/'+project_id, '_blank');
    win.focus();
}
