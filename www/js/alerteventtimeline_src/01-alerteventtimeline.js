
/*
 * Author: ikki Chung
 * Date: 01 Feb 2018
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var glWebSocketPort = 8083;
var glMenuCreated = false;
var glWebSocketState_alertevent = 1;// 1:disconected 0:connected

load_websocket_event();

function load_websocket_event()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        glWebSocketPort = data.web_socket_port;
        load_AlertEvent_websocket();

        //Pace.stop();
    });
}


function load_AlertEvent_websocket()
{
    //var wsUri = "ws://"+window.location.hostname+":"+glWebSocketPort+"/websocket/tag_0_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    //var wsUri = "ws://"+window.location.hostname+":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    // var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    // var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    var wsUri = "";
    if (gl_maingroups.length <= 3 && gl_maingroups != '1')
    {
        wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/maingroups_"+gl_maingroups+"_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    }
    else
    {
        wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|event|sos|fenceAlert|checkpointAlert";
    }

    console.log("load_AlertEvent_websocket() wsUri:"+wsUri);

    websocket = new WebSocket(wsUri);
    websocket.onmessage = function(evt) { onMessageAlertEvent(evt) };
    websocket.onclose = function(evt) { onCloseAlertEvent(evt) };
    websocket.onopen = function(evt) { onOpenAlertEvent(evt) };

    //Pace.stop();
}

function onMessageAlertEvent(evt)
{
    var data = JSON.parse(evt.data);

    //console.log("onMessageAlertEvent() data:"+data);

    //檢查訊息類型
    if(!data.hasOwnProperty('msgType'))
        return;

    if(data.msgType == 'sos')
    {
        console.log("onMessageAlertEvent() !! sos !! " +  data.tagid +", "+data.tagname);

        //如果訊息類型為sos， 則為求助訊息
        //data.
        var alert = 0;
        var alert0D = 0;
        if (data.type == 1)
            alert = 1;
        if (data.type == 2)
            alert0D = 1;

        //createHelpMsg(alert, alert0D, data.tagid, data.tagname, data.locatorid, data.locatorname);
        show_help_msg(alert, alert0D, data.tagid, data.tagname, data.locatorid, data.locatorname);
    }
    else
    if(data.msgType == 'event')
    {
        //console.log("onMessageAlertEvent() !! Event !!   call check_new_fired_events()");
        if (data.alertAction == '3')
        {
            console.log("onMessageAlertEvent() !! Event !! data.alertAction == alert ");
        }
        else
        if (data.alertAction == '4')
        {
            console.log("onMessage() !! Event !! data.alertAction == cancel data.nodeid:" + data.nodeid);

            remove_alert_box(data.nodeid, 0, -1);
        }
    }
    else
    if(data.msgType == 'fenceAlert')
    {
        //如果訊息類型為fenceAlert， 則為標籤觸發了電子圍欄報警
        console.log("onMessage() tagid:" + data.tagid +" mac:"+data.mac+" action:"+data.action+" areaid:"+data.areaid+" areaname:"+data.areaname+" type:"+data.type);
        //data.action;// 1:Enter 2:Leave
        //show_fence_alert_msg(data.id, data.action, data.areaname);

        createFenceAlertMsg(data.tagid, data.tagname, data.action, data.areaname);

    }
    else
    if(data.msgType == 'checkpointAlert')
    {
        //console.log("onMessageCheckPoint() chcekPointName:" + data.chcekPointName +" alertType:"+data.alertType);
        //data.action;// 1:Enter 2:Leave
        show_checkpoint_alert_msg(data.chcekPointName, data.alertType);
    }
    else
    if(data.msgType == 'callout')
    {
        // CallOut
        create_CallOut_box(data.nodeid, data.nodename);
    }

    parent.check_new_fired_events_countdown(10);

    refresh_timeline();

}

function onOpenAlertEvent(evt)
{
    console.log("onOpenAlertEvent() Connected to VILS Center!! ");
    var new_html = '<a><i class="fa fa-chain text-green"></i>';
    new_html += 'AlertEvent WebSocket Connected.</a>';

    parent.$('.top_warning_content_alertevent_websocket').html(new_html);

    glWebSocketState_alertevent = 0; // 1:disconected 0:connected
    update_top_warning_state();
}

function onCloseAlertEvent(evt)
{
    console.log("onCloseAlertEvent() reconnect to VILS Center!! ");

    var new_html = '<a><i class="fa fa-chain-broken text-red"></i>';
    new_html += 'AlertEvent WebSocket Connecting...</a>';

    parent.$('.top_warning_content_alertevent_websocket').html(new_html);

    glWebSocketState_alertevent = 1; // 1:disconected 0:connected
    update_top_warning_state();


    // reconnect
    setTimeout(load_AlertEvent_websocket, 2000);
}

function update_top_warning_state()
{
    var state = glWebSocketState_alertevent;

    parent.$("#top_warning_num").html('You have '+state+' warnings');
    parent.$("#top_warning_num_bubble").html(state);

    if ( state == 0 )
    {
        parent.$("#top_warning_num_bubble").hide();
    }
    else
    {
        parent.$("#top_warning_num_bubble").show();
    }
}

// create_alert_box('12F-05 李先生', '按下警急按鈕');

function create_alert_box(tagid, tagname, showMsg, alert_type)
{
    //
    // check if already existed!
    //
    var new_box_id = "alert_box_"+tagid+"_"+alert_type;
    var new_box = document.getElementById(new_box_id);
    if ( new_box != null )
    {
        //console.log(new_box  + " != null");
        return false;
    }

    console.log("create_alert_box() new_box_id:"+new_box_id);

    var alert_box_content = document.getElementById("alert-box-content");
    if ( alert_box_content == null )
    {
        console.log("alert_box_content == null");
        return false;
    }

    var box = document.createElement('div');
    box.setAttribute("id",new_box_id);
    box.setAttribute("class","box box-danger");

    var box_header = document.createElement('div');
    if (alert_type == 5)
    {
        box_header.setAttribute("class","box-header with-border bg-yellow");
    }
    else
    {
        box_header.setAttribute("class","box-header with-border bg-orange");
    }
    box.appendChild(box_header);

    var box_title = document.createElement('h3');
    box_title.setAttribute("class","box-title");
    box_title.textContent = tagname;
    box_header.appendChild(box_title);

    var box_tool = document.createElement('div');
    box_tool.setAttribute("class","box-tools pull-right");
    box_header.appendChild(box_tool);

    var box_tool_button = document.createElement('button');
    box_tool_button.setAttribute("type","button");
    box_tool_button.setAttribute("class","btn btn-box-tool bg-gray disabled");
    box_tool_button.setAttribute("data-widget","remove");
    box_tool_button.setAttribute("onclick","remove_alert_box(\""+tagid+"\", 1, "+alert_type+")");
    box_tool.appendChild(box_tool_button);

    var box_tool_button_icon = document.createElement('i');
    box_tool_button_icon.setAttribute("class","fa fa-times");
    box_tool_button.appendChild(box_tool_button_icon);


    var box_body = document.createElement('div');
    if (alert_type == 1)
    {
        // tag
        box_body.setAttribute("class","box-body bg-light-blue-active");
    }
    else
    if (alert_type == 2)
    {
        // locator
        box_body.setAttribute("class","box-body bg-maroon disabled");
    }
    else
    if (alert_type == 5)
    {
        // locator callout
        box_body.setAttribute("class","box-body bg-green disabled");
    }
    else
    {
        // fence alert
        box_body.setAttribute("class","box-body bg-maroon disabled");
    }
    box.appendChild(box_body);

    var box_msg = document.createElement('h1');
    box_msg.setAttribute("class","box-title");
    //box_msg.setAttribute("style","font-size: 800%;");
    box_msg.setAttribute("style","font-size:"+ ALERT_BOX_FONT_SIZE * 100 +"%;");
    box_msg.textContent = tagname;
    box_body.appendChild(box_msg);

    var box_msg_2 = document.createElement('h1');
    box_msg_2.setAttribute("class","box-title");
    box_msg_2.setAttribute("style","font-size: 200%;");
    box_msg_2.textContent = showMsg;
    box_body.appendChild(box_msg_2);

    alert_box_content.appendChild(box);


    //
    // info 2dmap
    //
    if ( parent.$('#contentIframe')[0] != undefined )
    {
        if ( typeof parent.$('#contentIframe')[0].contentWindow.start_alert === "function")
        {
            parent.$('#contentIframe')[0].contentWindow.start_alert(tagid);
        }
    }

    return true;
}

function remove_alert_box(tagid, uiclose, alert_type)
{
    if (alert_type == -1)
    {
        // remove all type
        var i;
        for(i=1;i<=6;i++)
        {
            var box_id = "alert_box_"+tagid+"_"+i;
            // console.log("remove_alert_box() box_id:"+box_id);
            var alert_box = document.getElementById(box_id);
            if (alert_box != null)
            {
                alert_box.remove();
            }
        }
    }
    else
    {
        var box_id = "alert_box_"+tagid+"_"+alert_type;
        // console.log("remove_alert_box() box_id:"+box_id);

        var alert_box = document.getElementById(box_id);
        if (alert_box != null)
        {
            alert_box.remove();
        }
    }

    if (alert_type == 1 || alert_type == 2 || alert_type == -1)
        stop_help_msg(tagid, uiclose);
    else
        stop_fencealert_msg(tagid, uiclose);

    console.log("remove_alert_box() accountid:"+accountid+", accountname:"+accountname+" tagid:"+tagid  +" alert_type:"+alert_type);

    if (uiclose == 1)// 使用者利用網頁關閉
    {
        var index;
        for(index=0;index<gl_allevents.length;index++)
        {
            var event = gl_allevents[index];
            if ( tagid === event.eventjson.nodeid && event.eventjson.action === 'Alert' && event.empid.length == 0)
            {
                console.log("remove_alert_box() index:"+index+", firedeventid:"+event.firedeventid +" nodeid:"+ event.eventjson.nodeid + " action:"+event.eventjson.action +" empid:"+event.empid);

                //
                // send t oserver
                //
                update_FiredEvents_close(event.firedeventid, tagid, accountid, alert_type);
                break;
            }
        }
    }

    //
    // info 2dmap
    //
    if ( parent.$('#contentIframe')[0] != undefined )
    {
        if ( typeof parent.$('#contentIframe')[0].contentWindow.stop_alert === "function")
        {
            parent.$('#contentIframe')[0].contentWindow.stop_alert(tagid);
        }
    }
}


function update_FiredEvents_close(firedeventid, tagid, empid, alert_type)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_FIREDEVENT_CLOSE'] = 1;
    jsondata['ID'] = firedeventid;
    jsondata['DEVICEID'] = tagid;
    jsondata['EMPID'] = empid;
    jsondata['TYPE'] = alert_type;

    var targetURL = gl_target_server + "/php/eventfire.php";

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


function convert_action_name(action_type)
{
    if (action_type == "Enter")
    {
        return "進入";
    }
    else
    if (action_type == "Leave")
    {
        return "離開";
    }
    else
    if (action_type == "Stay")
    {
        return "停留";
    }
    else
    if (action_type == "Alert")
    {
        return "按下警急按鈕";
    }
    else
    if (action_type == "AlertTagNotMoving")
    {
        return "無動作警告";
    }
    else
    if (action_type == "AlertLayDown")
    {
        return "躺臥警告";
    }
    else
    if (action_type == "Cancel" || action_type == "cancel")
    {
        return "按下取消按鈕";
    }
    else
    if (action_type == "Finish")
    {
        return "完成";
    }
    else
    if (action_type == "Exceed")
    {
        return "超過巡檢時間";
    }
    else
    if (action_type == "shorttime")
    {
        return "巡檢時間過短";
    }
    else
    if (action_type == "longtime")
    {
        return "巡檢時間過長";
    }
    else
    if (action_type == "CallOut")
    {
        return "按下呼換鈕";
    }
    else
    if (action_type == "PowerOff")
    {
        return "拔除電源";
    }
    else
    if (action_type == "PowerOn")
    {
        return "接通電源";
    }
    else
    if (action_type == "OpenDoor")
    {
        return "開門";
    }
    else
    if (action_type == "CloseDoor")
    {
        return "關門";
    }
    else
    if (action_type == "iCoverOpenDoor")
    {
        return "異常移動";
    }
    else
    if (action_type == "iCoverCloseDoor")
    {
        return "定時回報";
    }
    else
    if (action_type == "EnterDoor")
    {
        return "進入開門區域";
    }
    else
    if (action_type == "ExitDoor")
    {
        return "離開開門區域";
    }
    else
        return action_type;
}

function convert_IssueTime_name(IssueTime)
{
    if (IssueTime == "Immediately")
    {
        return "立刻";
    }
    else
    if (IssueTime == "1 minute")
    {
        return "1分鐘";
    }
    else
    if (IssueTime == "5 minutes")
    {
        return "5分鐘";
    }
    else
    if (IssueTime == "10 minutes")
    {
        return "10分鐘";
    }
    else
    {
        return IssueTime;
    }
}

function format_device_name(device_name)
{
    var fmt_name = device_name;

    // search '(' and ')'
    var left_brackets = device_name.indexOf('(');
    var right_brackets = device_name.indexOf(')');

    if ( left_brackets >= 0 && right_brackets >= 0)
    {
        if ( left_brackets > right_brackets)
        {
            // not a valid "(xxx)"  maybe is  ")ff("
        }
        else
        {
            var str_len = device_name.length;

            if ( left_brackets == 0 )
            {
                // "(xxx)ccc"
                var first_name = device_name.substr(left_brackets, right_brackets - left_brackets + 1);
                var second_name = device_name.substr(right_brackets + 1);

                // console.log("format_device_name() 1 device_name:"+device_name+", first_name:"+first_name + ", second_name:"+second_name);

                var fmt_name = first_name + "<br>" + second_name;
                return fmt_name;
            }
            else
            {
                // "ccc(xxx)"
                var first_name = device_name.substr(0, left_brackets);
                var second_name = device_name.substr(left_brackets);

                // console.log("format_device_name() 2 device_name:"+device_name+", first_name:"+first_name + ", second_name:"+second_name);

                var fmt_name = first_name + "<br>" + second_name;
                return fmt_name;
            }
        }
    }

    //
    // search first number
    //
    var i;
    var first_digit = -1;
    var second_digit = -1;
    for (i=0;i<device_name.length;i++)
    {
        var char = device_name.substr(i, 1);
        if ( isANumber(char) || (char == '-') )
        {
            if (first_digit == -1)
                first_digit = i;
            else
                second_digit = i;
        }
        else
        if (first_digit >= 0 && second_digit >= 0 )
        {
            // found not valid digit
            break;
        }
    }

    //console.log("first_digit:"+first_digit+", second_digit:"+second_digit);

    var first_name = "";
    var second_name = "";
    if ( first_digit == 0 )
    {
        // "01-12xxx"
        first_name = device_name.substr(first_digit, second_digit - first_digit + 1);
        second_name = device_name.substr(second_digit + 1);

    }
    else
    if ( first_digit < 0 )
    {
        first_name = device_name;
        second_name = "";
    }
    else
    {
        // "xx01-12x"
        first_name = device_name.substr(0, first_digit);
        second_name = device_name.substr(first_digit);
    }
    // console.log("format_device_name() 3 device_name:"+device_name+", first_name:"+first_name + ", second_name:"+second_name);
    // console.log("format_device_name() 3 first_digit:"+first_digit+", second_digit:"+second_digit);

    var fmt_name = first_name + "<br>" + second_name;
    if (second_name === "" )
    {
        fmt_name = first_name;
    }
    return fmt_name;

}

function isANumber(str)
{
    return !/\D/.test(str);
}

function open_eventtimeline_page()
{
    var win = window.open('/index.php/vilseventtimeline/'+project_id, '_blank');
    win.focus();
}
