/*
 * Author: ikki Chung
 * Date: 07 Sep 2016
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

var glTag_List = [];
var gl_tag_num = 0;
var gl_tag_load_index = 0;
var LOAD_DEVICE_NUM = 20;

function event_trig_tag_select(eventidx, itemidx)
{
    var select_value = $("#event_trig_tag_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#event_trig_tag_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#event_trig_tag_"+eventidx+"_name").html(select_value);
    $("#event_trig_tag_"+eventidx+"_name").attr("data-id", select_id);
}

function event_trig_action_select(eventidx, itemidx)
{
    var select_value = $("#event_trig_action_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#event_trig_action_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#event_trig_action_"+eventidx+"_name").html(select_value);
    $("#event_trig_action_"+eventidx+"_name").attr("data-id", select_id);
}

function event_trig_area_select(eventidx, itemidx)
{
    var select_value = $("#event_trig_area_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#event_trig_area_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#event_trig_area_"+eventidx+"_name").html(select_value);
    $("#event_trig_area_"+eventidx+"_name").attr("data-id", select_id);
}

function event_trig_time_select(eventidx, itemidx)
{
    var select_value = $("#event_trig_time_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#event_trig_time_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#event_trig_time_"+eventidx+"_name").html(select_value);
    $("#event_trig_time_"+eventidx+"_name").attr("data-id", select_id);
}


function event_trig_delete(event_id)
{

    //alert("event_trig_delete() eventidx = "+ eventidx +", event_id = "+ event_id);


    var retVal = confirm("Do you want to delete this event ?");
    if( retVal == true )
    {
        // remove UI button group
        $("#event_trig_"+event_id).remove();
        $("#event_trig_delete_"+event_id).remove();

        var SendData = "eventid="+event_id;
        SendData += "&project_id="+project_id;
        SendData += "&delete_event=1";

        return jQuery.ajax({
         url: '/php/events.php',
         type: 'POST',
         data: SendData,
         dataType: 'text',
         // return jQuery.ajax({
         //  url: '/php/events.php?eventid='+event_id+'&project_id='+project_id,
         //  type: 'delete',
         //  contentType: "application/json",
         //  dataType: "json",
         //  data: {},
          success:
          	function(result)
          	{
                //alert("event_trig_delete success, event_id="+event_id+",result="+result.eventid);
    		},
          error:
          	function(xhr, status, error)
          	{
    			alert(error);
    		    //alert(xhr.responseText);
                window.location =  "/index.php/events/"+project_id;
    		}
        });

    }
    else
    {
        return false;
    }



}

function event_trig_create()
{
    var nodeid = $("#event_trig_tag_0_name").attr("data-id");
    var actionid = $("#event_trig_action_0_name").attr("data-id");
    var areaid = $("#event_trig_area_0_name").attr("data-id");
    var issuetimeid = $("#event_trig_time_0_name").attr("data-id");

    var json = {};
    json['project_id'] = ''+project_id;
    json['nodeid'] = nodeid;
    json['action'] = actionid;
    json['areaid'] = areaid;
    json['issuetime'] = issuetimeid;

    //alert('nodeid='+nodeid+", actionid="+actionid+", areaid="+areaid+", issuetimeid="+issuetimeid);
    //alert(JSON.stringify(json));

    return jQuery.ajax({
    url: '/php/events.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
    	function(response)
    	{
            //alert(JSON.stringify(response));
            load_event_exist();
            //window.location =  "/index.php/events/"+project_id;
        },
    error:
    	function(xhr, status, error)
    	{
    	   alert(error);
           alert(xhr.responseText);
        }
    });

}


function add_exist_event(eventkey, nodeid, nodename, action, actionname, areaid, areaname, issuetime, issuetimename)
{
    var event_box_body= document.getElementById('event_box_body');

    var event_breadcrumb = document.createElement('div');
    event_box_body.appendChild(event_breadcrumb);
    event_breadcrumb.setAttribute("class","breadcrumb flat");
    event_breadcrumb.id = "event_trig_"+eventkey;

    // Tag name
    var btn_tag = document.createElement('a');
    event_breadcrumb.appendChild(btn_tag);
    btn_tag.setAttribute("class","active");
    //btn_tag.setAttribute("href", "javascript:;");
    //btn_tag.setAttribute("onclick", "event_trig_tag_name();return false;");
    var btn_tag_text = document.createTextNode(nodename);
    btn_tag.appendChild(btn_tag_text);


    // Action name
    var btn_action = document.createElement('a');
    event_breadcrumb.appendChild(btn_action);
    //btn_action.setAttribute("class","active");
    //btn_action.setAttribute("href", "javascript:;");
    //btn_tag.setAttribute("onclick", "event_trig_edit("+index+", eventid);return false;");
    var btn_action_text = document.createTextNode(actionname);
    btn_action.appendChild(btn_action_text);

    // Area name
    var btn_area = document.createElement('a');
    event_breadcrumb.appendChild(btn_area);
    var btn_area_text = document.createTextNode(areaname);
    btn_area.appendChild(btn_area_text);

    // issueTime name
    var btn_issuetime = document.createElement('a');
    event_breadcrumb.appendChild(btn_issuetime);
    var btn_issuetime_text = document.createTextNode(issuetimename);
    btn_issuetime.appendChild(btn_issuetime_text);


    //
    // editor buttons
    //
    var btn_group_tools = document.createElement('div');
    btn_group_tools.setAttribute("class","btn-group pull-right");
    event_box_body.appendChild(btn_group_tools);

    var btn_tools = document.createElement('div');
    btn_tools.setAttribute("class","tools");
    btn_group_tools.appendChild(btn_tools);

    // delete button
    var btn_tools_delete = document.createElement('a');
    btn_tools.appendChild(btn_tools_delete);
    btn_tools_delete.setAttribute("href", "javascript:;");
    btn_tools_delete.setAttribute("onclick", "event_trig_delete("+eventkey+");return false;");
    btn_tools_delete.setAttribute("data-id", "eventid");
    btn_tools_delete.id = "event_trig_delete_"+eventkey;
    var btn_tools_delete_img = document.createElement('i');
    btn_tools_delete_img.setAttribute("class", "fa fa-trash-o");
    btn_tools_delete.appendChild(btn_tools_delete_img);

    var br1 = document.createElement("br");
    event_box_body.appendChild(br1);

}

function load_event_exist()
{
    var targetURL = gl_target_server + "/php/events.php";
    $.getJSON(targetURL, 'loadevents=1'+'&project_id='+project_id, function(data)
    {
        var event_box_body= document.getElementById('event_box_body');
        while (event_box_body.firstChild)
        {
            event_box_body.removeChild(event_box_body.firstChild);
        }

        var index = 1;
        var events = [];
        $.each(data, function(eventkey, value)
        {
            //events.push(eventkey);
            //alert("eventkey="+eventkey);
            add_exist_event(eventkey,
                value.nodeid, value.nodename,
                value.action, value.action,
                value.areaid, value.areaname,
                value.issuetime, value.issuename);
            index++;
        });


        //alert("events.length="+events.length);

        //add_exist_event(1);


    });
    return false;
}

function load_Items_to_newevent_UI(itemtype, uitype, tag_selection_btn_group, items)
{
    //var tags = [];
    //var index = 0;
    var itemid = "";
    var itemname = "";

    if ( items.length > 0 )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = "event_trig_"+itemtype+"_0_name";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    //btn_group1_button2.setAttribute("style", "display: none;");
    btn_group1_button2.id = "event_trig_"+itemtype+"_0";


    var btn_group1_button2_span1 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span1);
    btn_group1_button2_span1.setAttribute("class","caret");

    var btn_group1_button2_span2 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span2);
    btn_group1_button2_span2.setAttribute("class","sr-only");
    //var btn_group1_button2_span2_text = document.createTextNode("Toggle Dropdown");
    //btn_group1_button2_span2.appendChild(btn_group1_button2_span2_text);

    var dropdown_ul = document.createElement('ul');
    tag_selection_btn_group.appendChild(dropdown_ul);
    dropdown_ul.setAttribute("class", "dropdown-menu");
    dropdown_ul.setAttribute("role", "menu");
    //dropdown_ul.id = "dropdown_menu";

    for (var i = 0; i < items.length; i++)
    {
        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "event_trig_"+itemtype+"_select(0,"+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = "event_trig_"+itemtype+"_0_item_"+i;
        var dropdown_li_0_href_text = document.createTextNode(items[i].itemname);
        dropdown_li_0_href.appendChild(dropdown_li_0_href_text);
    }

}

function load_Tags_Info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_tag_load_index = 0;
    })
    .success(function() {
        setTimeout(load_Tags, 10);
    })
    .error(function() {
        setTimeout(load_Tags_Info, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_Tags()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
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
            // split data
            var node = data[key];
            var nodeData = node.split(",");
            //alert(nodeData[3]);
            glTag_List[i] = {};
            glTag_List[i].itemid = nodeData[3];
            glTag_List[i].itemname = nodeData[5];
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            var tag_selection_btn_group = document.getElementById('event_tag_selection');
            load_Items_to_newevent_UI("tag", "btn-info", tag_selection_btn_group, glTag_List);
        }
        else
        {
            setTimeout(load_Tags, 10);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_Tags, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function load_Tags_old()
{
    var tag_selection_btn_group = document.getElementById('event_tag_selection');
    var tags = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadtag=1'+'&project_id='+project_id, function(data)
    {
        var srv_tag_num = parseInt(data['LOC_TAG_NUM']);

        for (var i = 0; i < srv_tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            var nodeData = node.split(",");
            //alert(nodeData[3]);
            tags[i] = {};
            tags[i].itemid = nodeData[3];
            tags[i].itemname = nodeData[5];
        }

        load_Items_to_newevent_UI("tag", "btn-info", tag_selection_btn_group, tags);
    });
}

function load_Actions()
{
    var action_selection_btn_group = document.getElementById('event_action_selection');
    var actions = [];

    var targetURL = gl_target_server + "/php/events.php";
    $.getJSON(targetURL, 'loadactions=1'+'&project_id='+project_id, function(data)
    {
        for (var i = 0; i < data.length; ++i)
        {

            var actionname = convert_action_name(data[i].actionname);
            if (actionname == "")
                continue;

            actions[i] = {};
            actions[i].itemid = data[i].actionid;
            actions[i].itemname = actionname;
        }
        load_Items_to_newevent_UI("action", "btn-warning", action_selection_btn_group, actions);
    });

    //actions[0] = {};
    //actions[0].itemid = "action1";
    //actions[0].itemname = "Enter";
    //actions[1] = {};
    //actions[1].itemid = "action2";
    //actions[1].itemname = "Leave";
}

function load_Areas()
{
    var area_selection_btn_group = document.getElementById('event_area_selection');
    var areas = [];

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'project_id='+project_id, function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, area)
        {
            var area_json = JSON.parse(area);
            //alert('itemindex='+itemindex+", area_json.properties.areaid="+area_json.properties.areaid);
            areas[index] = {};
            areas[index].itemid = area_json.properties.areaid;
            areas[index].itemname = area_json.properties.areaname;

            //alert('itemindex='+itemindex+", area.areaid="+area_json['areaid']);

            index++;
        });

        load_Items_to_newevent_UI("area", "btn-success", area_selection_btn_group, areas);
    });

    //areas[0] = {};
    //areas[0].itemid = "area1";
    //areas[0].itemname = "Area 1";
    //areas[1] = {};
    //areas[1].itemid = "area2";
    //areas[1].itemname = "Area 2";
}

function load_IssueTimes()
{
    var time_selection_btn_group = document.getElementById('event_time_selection');
    var times = [];

    var targetURL = gl_target_server + "/php/events.php";
    $.getJSON(targetURL, 'loadissuetimes=1'+'&project_id='+project_id, function(data)
    {
        for (var i = 0; i < data.length; ++i)
        {
            times[i] = {};
            times[i].itemid = data[i].issueid;
            times[i].itemname = data[i].issuename;
        }
        load_Items_to_newevent_UI("time", "btn-danger", time_selection_btn_group, times);
    });

    // jump to first item
    event_trig_time_select();

}

function load_event_conditions()
{
    //load_Tags();
    load_Tags_Info();
    load_Actions();
    load_Areas();
    load_IssueTimes();
    return false;
}

load_event_exist();

//add_exist_event(1, "tag1_id", "tag1", "enter", "enter", "areaid", "areaname", "issuetime", "Immediately");
//add_exist_event(2, "tag2_id", "tag2", "Leave", "enter", "areaid", "areaname", "issuetime", "Immediately");
load_event_conditions();


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
    if (action_type == "Cancel" || action_type == "cancel")
    {
        return "按下取消按鈕";
    }
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

function open_events_page()
{
    var win = window.open('/index.php/vilsevents/'+project_id, '_blank');
    win.focus();
}
