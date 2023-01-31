/*
 * Author: ikki Chung
 * Date: 2020 07 16
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glWaitWebSocket = 0;

var gl_user_info = [];
var gl_nfclocators = [];
var glUserBodyRecords = [];

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;
var gl_LoadSingleUser = '';

var glStartDate = moment().subtract(29, 'days');
var glEndDate = moment();

check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
     case 0:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             var recorder_user = document.getElementById('recorder_user');
             recorder_user.textContent = gl_accountname + ' ['+ gl_accountid +']';

             var cur_date = moment().format("YYYY-MM-DD HH:mm:ss");
             var bodyinfo_datetime = document.getElementById('bodyinfo_datetime');
             bodyinfo_datetime.value = cur_date;

             gl_user_info = [];
             gl_user_load_index = 0;
             load_user_info();
         }
     break;
     case 1:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             create_user_dropdown_list();

             CurrentLoadIndex = CurrentLoadIndex + 1;
         }
     break;
     case 2:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             load_nfclocator();
         }
     break;
     case 3:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             create_chart();
             load_user_bodyinfo_records(gl_accountid);
             CurrentLoadIndex = CurrentLoadIndex + 1;
         }
     break;
     case 4:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             wait_websocket_event_cardrollcall();

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


function load_user_info()
{
    var user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);

            user_num += 1;
        });

        gl_user_load_index += user_num;
    })
    .success(function() {
        if (user_num < LOAD_USER_NUM)
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_user_info, 20);
        }
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function create_user_dropdown_list()
{
    var target_user = document.getElementById('target_user');

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    target_user.appendChild(div_btngroup);

    var first_userinfo = gl_user_info[0];

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.setAttribute("id","dropdown_current_user");
    div_button.setAttribute("value",first_userinfo.account);
    div_button.textContent = first_userinfo.name + '['+ first_userinfo.account +']';
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_btngroup.appendChild(div_ul);

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var userinfo = gl_user_info[i];

        div_ul.appendChild(create_dropdown_user_item(userinfo));
    }

    // space
    var div_space  = document.createElement("div");
    var Spanspace = document.createElement("span");
    Spanspace.setAttribute("style","display:inline;padding:.0em .1em .1em;font-size:50%;font-weight:100;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
    Spanspace.textContent = "  ";
    div_space.appendChild(Spanspace);
    target_user.appendChild(div_space);

    // 照片
    var div_photo = document.createElement("div");
    var Span_photo = document.createElement("span");
    Span_photo.setAttribute("id","uiid_photo");
    div_photo.appendChild(Span_photo);
    target_user.appendChild(div_photo);
}

function create_dropdown_user_item(userinfo)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", userinfo.name);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+ userinfo.name +"\", \""+ userinfo.account +"\", \""+ userinfo.imageuid +"\");return true;");

    item_click.appendChild(document.createTextNode(userinfo.name + '['+ userinfo.account +']'));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_user_click(name, accountid, imageuid)
{
    var div_button = document.getElementById("dropdown_current_user");
    div_button.setAttribute("value", accountid);
    div_button.textContent = name + '['+ accountid +']';

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    load_user_bodyinfo_records(accountid);

}

function load_user_photo(imageuid)
{
    console.log("load_user_photo() imageuid:"+imageuid);

    var ui_id = 'uiid_photo';
    var show_img_area_image_ui = document.getElementById(ui_id);
    if ( show_img_area_image_ui != undefined )
    {
        while (show_img_area_image_ui.firstChild)
        {
            show_img_area_image_ui.removeChild(show_img_area_image_ui.firstChild);
        }
    }

    if ( imageuid.length == 0 )
    {
        if ( show_img_area_image_ui != undefined )
        {
            var span_icon_img = document.createElement("img");
            span_icon_img.setAttribute("src","/images/userimage.png");
            span_icon_img.setAttribute("style","max-width: 120px;");
            show_img_area_image_ui.appendChild(span_icon_img);
        }
    }
    else
    {
        var targetURL = gl_target_server + "/php/storage.php";
        var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
        {
            var ui_id = 'uiid_photo';
            var show_img_area_image_ui = document.getElementById(ui_id);
            if ( show_img_area_image_ui != undefined )
            {
                while (show_img_area_image_ui.firstChild)
                {
                    show_img_area_image_ui.removeChild(show_img_area_image_ui.firstChild);
                }
                if ( show_img_area_image_ui != undefined )
                {
                    // console.log("load_user_photo() storageImage.accountid:"+ storageImage.accountid +" storageImage.id:"+storageImage.id);
                    var user_photo = create_user_photo(storageImage);
                    show_img_area_image_ui.appendChild(user_photo);
                }
            }
        })
        .success(function() {
        })
        .error(function() {
        })
        .complete(function() {
        });
    }
}

function create_user_photo(storageImage)
{
    var span_icon_img = document.createElement("img");
    span_icon_img.setAttribute("src","/php/downloadfile.php?loadthumb=1&project_id="+project_id+"&imageid="+storageImage.id);
    span_icon_img.setAttribute("alt",storageImage.description);
    span_icon_img.setAttribute("style","max-width: 120px;");
    return span_icon_img;
}

function load_nfclocator()
{
    if (gl_nfclocators.length > 0 )
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
        });

    })
    .success(function() {

        put_nfcdevice_to_table();

        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function put_nfcdevice_to_table()
{
    var nfcdevice_dropdown_menu = document.getElementById('nfcdevice_dropdown_menu');

    // select all button
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_selectall(\"device_nfclocator\");return true;");
    item_li.appendChild(item_click);
    item_click.appendChild(document.createTextNode("全選"));
    nfcdevice_dropdown_menu.appendChild(item_li);

    //var idList = idlist.split(",");

    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfcdevice = gl_nfclocators[i];
        // var ret = idList.indexOf(nfcdevice.macaddress);
        var existed = true;
        // if (ret >= 0)
        //     existed = true;
        nfcdevice_dropdown_menu.appendChild(create_dropdown_device_item("nfclocator", nfcdevice.deviceid, parseInt(nfcdevice.macaddress), nfcdevice.name, existed));
    }
}

function create_dropdown_device_item(prefix, deviced, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_device_click(\""+prefix+"\","+ deviced +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_device_"+prefix+"_"+deviced);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_device_click();return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_selectall(prefix)
{
    // console.log("dropdown_selectall() prefix:"+prefix);

    if( prefix === 'device_uhfreader')
    {
        for (var i = 0; i < gl_rfiddevices.length; ++i)
        {
            var rfiddevice = gl_rfiddevices[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+rfiddevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
        }
    }
    else
    if( prefix === 'device_nfclocator')
    {
        for (var i = 0; i < gl_nfclocators.length; ++i)
        {
            var nfcdevice = gl_nfclocators[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+nfcdevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
        }
    }
}

function dropdown_checkbox_device_click(index)
{
    glDropdown_checkbox_device_click = true;
}

var glDropdown_checkbox_device_click = false;
function dropdown_device_click(prefix, index)
{
    if (glDropdown_checkbox_device_click == true)
    {
        glDropdown_checkbox_device_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked = true;

    glDropdown_checkbox_device_click = false;
}

function save_bodyinfo()
{
    var current_user = document.getElementById("dropdown_current_user").value;
    var bodyinfo_datetime = document.getElementById("bodyinfo_datetime").value;
    var bodyinfo_weight = document.getElementById("bodyinfo_weight").value;
    var bodyinfo_heartbeat = document.getElementById("bodyinfo_heartbeat").value;
    var bodyinfo_bloodsugar = document.getElementById("bodyinfo_bloodsugar").value;
    var bodyinfo_systolicbloodpressure = document.getElementById("bodyinfo_systolicbloodpressure").value;
    var bodyinfo_diastolicbloodpressure = document.getElementById("bodyinfo_diastolicbloodpressure").value;
    var bodyinfo_bodytemperature = document.getElementById("bodyinfo_bodytemperature").value;
    var bodyinfo_bodyfat = document.getElementById("bodyinfo_bodyfat").value;
    var bodyinfo_bloodoxygen = document.getElementById("bodyinfo_bloodoxygen").value;

    console.log("current_user:"+current_user);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['SAVE_BODYINFO_ALL'] = 1;
    jsondata['accountid'] = current_user;
    jsondata['recorderid'] = gl_accountid;
    jsondata['weight'] = bodyinfo_weight;
    jsondata['heartbeat'] = bodyinfo_heartbeat;
    jsondata['bloodsugar'] = bodyinfo_bloodsugar;
    jsondata['systolicbloodpressure'] = bodyinfo_systolicbloodpressure;
    jsondata['diastolicbloodpressure'] = bodyinfo_diastolicbloodpressure;
    jsondata['bodytemperature'] = bodyinfo_bodytemperature;
    jsondata['bodyfat'] = bodyinfo_bodyfat;
    jsondata['bloodoxygen'] = bodyinfo_bloodoxygen;
    jsondata['datetime'] = bodyinfo_datetime;

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
                alert("儲存成功!!");
                load_user_bodyinfo_records(current_user);
            }

            //
            // clear data
            //
            var cur_date = moment().format("YYYY-MM-DD HH:mm:ss");

            document.getElementById("bodyinfo_datetime").value = cur_date;
            document.getElementById("bodyinfo_weight").value = "";
            document.getElementById("bodyinfo_heartbeat").value = "";
            document.getElementById("bodyinfo_bloodsugar").value = "";
            document.getElementById("bodyinfo_systolicbloodpressure").value = "";
            document.getElementById("bodyinfo_diastolicbloodpressure").value = "";
            document.getElementById("bodyinfo_bodytemperature").value = "";
            document.getElementById("bodyinfo_bodyfat").value = "";
            document.getElementById("bodyinfo_bloodoxygen").value = "";
        },
      error:
        function(xhr, status, error)
        {
            //alert("error = " + error);
            //alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

$("#savebodyinfobutton").click(function()
{
    save_bodyinfo();
    return false;
});


function create_chart()
{
    var show_chart_area = document.getElementById("show_chart_area");

    var weight_chart = create_chart_block("weight_chart", '體重紀錄', 'bg-teal-gradient');
    show_chart_area.appendChild(weight_chart);
    var heartbeat_chart = create_chart_block("heartbeat_chart", '心率紀錄', 'bg-aqua-gradient');
    show_chart_area.appendChild(heartbeat_chart);
    var bloodsugar_chart = create_chart_block("bloodsugar_chart", '血糖紀錄', 'bg-red-gradient disabled');
    show_chart_area.appendChild(bloodsugar_chart);
    var systolicbloodpressure_chart = create_chart_block("systolicbloodpressure_chart", '收縮壓紀錄', 'bg-yellow-gradient');
    show_chart_area.appendChild(systolicbloodpressure_chart);
    var diastolicbloodpressure_chart = create_chart_block("diastolicbloodpressure_chart", '舒張壓紀錄', 'bg-green-gradient');
    show_chart_area.appendChild(diastolicbloodpressure_chart);
    var bodytemperature_chart = create_chart_block("bodytemperature_chart", '體溫紀錄', 'bg-light-blue-gradient');
    show_chart_area.appendChild(bodytemperature_chart);
    var bodyfat_chart = create_chart_block("bodyfat_chart", '體脂率紀錄', 'bg-purple-gradient');
    show_chart_area.appendChild(bodyfat_chart);
    var bloodoxygen_chart = create_chart_block("bloodoxygen_chart", '血氧紀錄', 'bg-maroon-gradient');
    show_chart_area.appendChild(bloodoxygen_chart);

    glWeightLineChart = create_line_chart('weight_chart_line_chart', '體重', '#223377');
    glHeartBeatLineChart = create_line_chart('heartbeat_chart_line_chart', '心率', '#227744');
    glBloodSugarLineChart = create_line_chart('bloodsugar_chart_line_chart', '血糖', '#227744');
    glSystolicBloodPressureLineChart = create_line_chart('systolicbloodpressure_chart_line_chart', '收縮壓', '#227744');
    glDiastolicBloodPressureLineChart = create_line_chart('diastolicbloodpressure_chart_line_chart', '舒張壓', '#227744');
    glBodyTemperatureLineChart = create_line_chart('bodytemperature_chart_line_chart', '體溫', '#227744');
    glBodyfatLineChart = create_line_chart('bodyfat_chart_line_chart', '體脂率', '#227744');
    glBloodOxygenLineChart = create_line_chart('bloodoxygen_chart_line_chart', '血氧', '#227744');

}

function create_chart_block(uiid, title, bgcolor)
{
    // var chart = document.createElement('row');
    // chart.setAttribute("class", "row");
    // chart.setAttribute("style", "display:none;");
    // chart.setAttribute("id", uiid);

    var chart_col = document.createElement('div');
    chart_col.setAttribute("class", "col-xs-12 col-sm-12 col-md-6 col-lg-4");
    chart_col.setAttribute("id", uiid);
    // chart.appendChild(chart_col);

    var chart_col_box = document.createElement('div');
    chart_col_box.setAttribute("class", "box box-solid " + bgcolor);
    chart_col.appendChild(chart_col_box);

    var chart_col_box_header = document.createElement('div');
    chart_col_box_header.setAttribute("class", "box-header");
    chart_col_box.appendChild(chart_col_box_header);

    var chart_col_box_header_i = document.createElement('i');
    chart_col_box_header_i.setAttribute("class", "fa fa-th");
    chart_col_box_header.appendChild(chart_col_box_header_i);

    var chart_col_box_header_title = document.createElement('h3');
    chart_col_box_header_title.setAttribute("class", "box-title");
    chart_col_box_header_title.setAttribute("id", uiid + "_title");
    chart_col_box_header_title.textContent = title;
    chart_col_box_header.appendChild(chart_col_box_header_title);


    var chart_col_box_body = document.createElement('div');
    chart_col_box_body.setAttribute("class", "box-body border-radius-none");
    chart_col_box.appendChild(chart_col_box_body);

    var chart_col_box_body_chart = document.createElement('div');
    chart_col_box_body_chart.setAttribute("class", "chart");
    chart_col_box_body_chart.setAttribute("style", "height: 150px;");
    chart_col_box_body_chart.setAttribute("id", uiid + "_line_chart");
    chart_col_box_body.appendChild(chart_col_box_body_chart);

    return chart_col;
    // return chart;
}

function create_line_chart(uiid, show_label, lineColors)
{
    var uiLineChart = new Morris.Line({
      element          : uiid,
      resize           : true,
      redraw           : true,
      data             : [
        { y: '', item1: 0 },
        // { y: '2016-11-06 16:36:16', item1: 64 },
        // { y: '2016-10-31 16:36:16', item1: 66 }
      ],
      xkey             : 'y',
      ykeys            : ['item1'],
      labels           : [show_label],
      lineColors       : [lineColors],
      lineWidth        : 2,
      parseTime        : false,
      hideHover        : 'auto',
      pointSize        : 4,
      pointStrokeColors: [lineColors],
      gridTextColor    : '#222',
      gridStrokeWidth  : 0.4,
      gridIntegers     : true,
      gridLineColor    : '#efefef',
      gridTextFamily   : 'Open Sans',
      gridTextSize     : 10
    });

    return uiLineChart;
}

function wait_websocket_event_cardrollcall()
{
    if (glWaitWebSocket == 1 )
    {
        return;
    }

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        glWebSocketPort = data.web_socket_port;
        load_cardrollcall_websocket();
    });
}

function load_cardrollcall_websocket()
{
    var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|cardrollcallevent";

    console.log("load_cardrollcall_websocket() wsUri:"+wsUri);

    websocket = new WebSocket(wsUri);
    websocket.onmessage = function(evt) { onMessageCardRollCallEvent(evt) };
    websocket.onclose = function(evt) { onCloseCardRollCallEvent(evt) };
    websocket.onopen = function(evt) { onOpenCardRollCallEvent(evt) };
}

function onMessageCardRollCallEvent(evt)
{
    var data = JSON.parse(evt.data);

    // console.log("onMessageCardRollCallEvent() data:"+data);

    //檢查訊息類型
    if(!data.hasOwnProperty('msgType'))
        return;

    if(data.msgType == 'cardrollcallevent')
    {
        console.log("onMessageCardRollCallEvent() cardrollcallevent cardid:" +  data.cardid +" cardtype:" +  data.cardtype +" readerid:" +  data.readerid);

        show_card_user_bodyinfo(data.cardid, data.cardtype, data.readerid);
    }
}

function onOpenCardRollCallEvent(evt)
{
    console.log("onOpenCardRollCallEvent() Connected to VILS Center!! ");
    glWaitWebSocket = 1; // 0:disconected 1:connected
}

function onCloseCardRollCallEvent(evt)
{
    console.log("onCloseCardRollCallEvent() reconnect to VILS Center!! ");
    glWaitWebSocket = 0; // 0:disconected 1:connected

    // reconnect
    setTimeout(load_cardrollcall_websocket, 2000);
}

function show_card_user_bodyinfo(cardid, cardtype, readerid)
{
    // 檢查是否有被勾選
    var device_type = "uhfreader";
    if( cardtype == '0' )
        device_type = "nfclocator";
    var checkbox = document.getElementById("dropdown_checkbox_device_"+device_type+"_"+readerid);

    if (checkbox.checked)
    {
        load_card_user_bodyinfo(cardid, cardtype, readerid);
    }
}

function load_card_user_bodyinfo(cardid, cardtype, readerid)
{
    console.log("load_card_user_bodyinfo() cardid="+cardid);

    var bFound = false;
    var user_name = "無資料";
    var user_imageuid = '0';
    for (var i = 0; i < gl_user_info.length; i++)
    {
        var cur_user = gl_user_info[i];

        if( cardtype == '0' )
        {
            if ( cur_user.nfccards != undefined )
            {
                if( cur_user.nfccards.includes(cardid) )
                {
                    gl_LoadSingleUser = cur_user.account;
                    user_name = cur_user.name;
                    user_imageuid = cur_user.imageuid;
                    bFound = true;
                    break;
                }
            }
        }
        else
        if( cardtype == '1' )
        {
            if ( cur_user.rfidcards != undefined )
            {
                if( cur_user.rfidcards.includes(cardid) )
                {
                    gl_LoadSingleUser = cur_user.account;
                    user_name = cur_user.name;
                    user_imageuid = cur_user.imageuid;
                    bFound = true;
                    break;
                }
            }
        }
    }

    var device_name = "";
    if( cardtype == '0' )
    {
        var nfcdevice = getNFCDevice(readerid);
        device_name = nfcdevice.name;
    }
    var cardscan_result_label = document.getElementById("cardscan_result_label");

    if (bFound)
    {
        dropdown_user_click(user_name, gl_LoadSingleUser, user_imageuid);
        load_user_bodyinfo_records(gl_LoadSingleUser);
        cardscan_result_label.textContent = device_name+" 已讀取卡片:"+cardid+" 人員:"+user_name;
    }
    else
    {
        cardscan_result_label.textContent = device_name+" 已讀取卡片:"+cardid+" 人員:"+user_name;
    }
}


function load_user_bodyinfo_records(accountid)
{
    var StartDate = glStartDate.format("YYYY-MM-DD HH:mm:ss");

    glEndDate = moment();
    var EndDate = glEndDate.format("YYYY-MM-DD HH:mm:ss");

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodyinforecords=1'+'&accountid='+accountid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = {};

        $.each(data, function(itemindex, bodyinfo)
        {
            // console.log("load_user_bodyinfo_records() itemindex:"+itemindex+" bodyinfo:"+bodyinfo);
            bodyinfoList[itemindex] = bodyinfo;
        });
        glUserBodyRecords[accountid] = bodyinfoList;
    })
    .success(function() {
        show_user_bodyinfo_records(accountid);
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function show_user_bodyinfo_records(accountid)
{
    var user_body_info_table = document.getElementById('user_body_info_table');
    user_body_info_table.setAttribute("style",'display:block');

    var weight_chart = document.getElementById('weight_chart');
    var heartbeat_chart = document.getElementById('heartbeat_chart');
    var bloodsugar_chart = document.getElementById('bloodsugar_chart');
    var systolicbloodpressure_chart = document.getElementById("systolicbloodpressure_chart");
    var diastolicbloodpressure_chart = document.getElementById("diastolicbloodpressure_chart");
    var bodytemperature_chart = document.getElementById("bodytemperature_chart");
    var bodyfat_chart = document.getElementById("bodyfat_chart");
    var bloodoxygen_chart = document.getElementById("bloodoxygen_chart");

    var bodyinfoList = glUserBodyRecords[accountid];

    weight_chart.setAttribute("style",'display:none');
    heartbeat_chart.setAttribute("style",'display:none');
    bloodsugar_chart.setAttribute("style",'display:none');
    systolicbloodpressure_chart.setAttribute("style",'display:none');
    diastolicbloodpressure_chart.setAttribute("style",'display:none');
    bodytemperature_chart.setAttribute("style",'display:none');
    bodyfat_chart.setAttribute("style",'display:none');
    bloodoxygen_chart.setAttribute("style",'display:none');

    if (bodyinfoList == undefined || accountid === '')
    {
        return;
    }

    if (bodyinfoList.length == 1 )
    {
        return;
    }

    //
    // find user
    //
    var curUserInfo = null;
    for (var i = 0; i < gl_user_info.length; i++)
    {
        var user = gl_user_info[i];

        if ( accountid === user.account)
        {
            curUserInfo = user;
            break;
        }
    }

    if ( curUserInfo == null )
    {
        return;
    }

    load_user_photo(curUserInfo.imageuid);

    var weight_chart_title = document.getElementById("weight_chart_title");
    var heartbeat_chart_title = document.getElementById("heartbeat_chart_title");
    var bloodsugar_chart_title = document.getElementById("bloodsugar_chart_title");
    var systolicbloodpressure_chart_title = document.getElementById("systolicbloodpressure_chart_title");
    var diastolicbloodpressure_chart_title = document.getElementById("diastolicbloodpressure_chart_title");
    var bodytemperature_chart_title = document.getElementById("bodytemperature_chart_title");
    var bodyfat_chart_title = document.getElementById("bodyfat_chart_title");
    var bloodoxygen_chart_title = document.getElementById("bloodoxygen_chart_title");

    weight_chart_title.textContent = curUserInfo.name + ' 體重紀錄';
    heartbeat_chart_title.textContent = curUserInfo.name + ' 心率紀錄';
    bloodsugar_chart_title.textContent = curUserInfo.name + ' 血糖紀錄';
    systolicbloodpressure_chart_title.textContent = curUserInfo.name + ' 收縮壓紀錄';
    diastolicbloodpressure_chart_title.textContent = curUserInfo.name + ' 舒張壓紀錄';
    bodytemperature_chart_title.textContent = curUserInfo.name + ' 體溫紀錄';
    bodyfat_chart_title.textContent = curUserInfo.name + ' 體脂率紀錄';
    bloodoxygen_chart_title.textContent = curUserInfo.name + ' 血氧紀錄';

    var weightinfoList = bodyinfoList.weight;
    var heartbeatinfoList = bodyinfoList.heartbeat;
    var bloodsugarinfoList = bodyinfoList.bloodsugar;
    var systolicbloodpressureinfoList = bodyinfoList.systolicbloodpressure;
    var diastolicbloodpressureinfoList = bodyinfoList.diastolicbloodpressure;
    var bodytemperatureinfoList = bodyinfoList.bodytemperature;
    var bodyfatinfoList = bodyinfoList.bodyfat;
    var bloodoxygeninfoList = bodyinfoList.bloodoxygen;

    $("#weight_chart_line_chart").empty();
    $("#heartbeat_chart_line_chart").empty();
    $("#bloodsugar_chart_line_chart").empty();
    $("#systolicbloodpressure_chart_line_chart").empty();
    $("#diastolicbloodpressure_chart_line_chart").empty();
    $("#bodytemperature_chart_line_chart").empty();
    $("#bodyfat_chart_line_chart").empty();
    $("#bloodoxygen_chart_line_chart").empty();

    if (bodyinfoList.weight.length > 0)
    {
        glWeightLineChart = create_line_chart('weight_chart_line_chart', '體重', '#223377');
        adjust_chart_max_min(glWeightLineChart, bodyinfoList.weight);
        weight_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.heartbeat.length > 0)
    {
        glHeartBeatLineChart = create_line_chart('heartbeat_chart_line_chart', '心率', '#227744');
        adjust_chart_max_min(glHeartBeatLineChart, bodyinfoList.heartbeat);
        heartbeat_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bloodsugar.length > 0)
    {
        glBloodSugarLineChart = create_line_chart('bloodsugar_chart_line_chart', '血糖', '#227744');
        adjust_chart_max_min(glBloodSugarLineChart, bodyinfoList.bloodsugar);
        bloodsugar_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.systolicbloodpressure.length > 0)
    {
        glSystolicBloodPressureLineChart = create_line_chart('systolicbloodpressure_chart_line_chart', '收縮壓', '#227744');
        adjust_chart_max_min(glSystolicBloodPressureLineChart, bodyinfoList.systolicbloodpressure);
        systolicbloodpressure_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.diastolicbloodpressure.length > 0)
    {
        glDiastolicBloodPressureLineChart = create_line_chart('diastolicbloodpressure_chart_line_chart', '舒張壓', '#227744');
        adjust_chart_max_min(glDiastolicBloodPressureLineChart, bodyinfoList.diastolicbloodpressure);
        diastolicbloodpressure_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bodytemperature.length > 0)
    {
        glBodyTemperatureLineChart = create_line_chart('bodytemperature_chart_line_chart', '體溫', '#227744');
        adjust_chart_max_min(glBodyTemperatureLineChart, bodyinfoList.bodytemperature);
        bodytemperature_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bodyfat.length > 0)
    {
        glBodyfatLineChart = create_line_chart('bodyfat_chart_line_chart', '體脂率', '#227744');
        adjust_chart_max_min(glBodyfatLineChart, bodyinfoList.bodyfat);
        bodyfat_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bloodoxygen.length > 0)
    {
        glBloodOxygenLineChart = create_line_chart('bloodoxygen_chart_line_chart', '血氧', '#227744');
        adjust_chart_max_min(glBloodOxygenLineChart, bodyinfoList.bloodoxygen);
        bloodoxygen_chart.setAttribute("style",'display:block');
    }

    glWeightLineChart.redraw();
    glHeartBeatLineChart.redraw();
    glBloodSugarLineChart.redraw();
    glSystolicBloodPressureLineChart.redraw();
    glDiastolicBloodPressureLineChart.redraw();
    glBodyTemperatureLineChart.redraw();
    glBodyfatLineChart.redraw();
    glBloodOxygenLineChart.redraw();

    // jQueryKnob
    $('.knob').knob();

}

function adjust_chart_max_min(uiLineChart, datainfoList)
{
    var data_min = 200;
    var data_max = 0;
    var dataitemlist = [];

    for(var index = datainfoList.length - 1; index>=0; index--)
    {
        var datainfo = datainfoList[index];
        var item_value = parseFloat(datainfo.data);

        var info = [];
        info['item1'] = item_value.toFixed(2);
        info['y'] = datainfo.datetime;
        dataitemlist.push(info);

        // console.log("adjust_chart_max_min() index:"+index+"  data_min:"+data_min+" data_max:"+data_max+" item_value:"+item_value);

        if (data_min > item_value )
        {
            // console.log("adjust_chart_max_min() index:"+index+" data_min > item_value");
            data_min = item_value;
        }
        if (data_max < item_value )
        {
            // console.log("adjust_chart_max_min() index:"+index+" data_max < item_value");
            data_max = item_value;
        }
    }

    uiLineChart.options["ymax"] = parseInt(data_max) + 2;
    uiLineChart.options["ymin"] = parseInt(data_min) - 2;
    uiLineChart.setData(dataitemlist);
}

function getNFCDevice(deviceid)
{
    var nfcdevice = null;
    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var device = gl_nfclocators[i];
        if ( device.deviceid == deviceid )
        {
            nfcdevice = device;
            break;
        }
    }
    return nfcdevice;
}

function open_bodyinfomanualinput_page()
{
    var win = window.open('/index.php/vilsbodyinfomanualinput/'+project_id, '_blank');
    win.focus();
}
