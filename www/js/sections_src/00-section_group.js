/*
 * Author: ikki Chung
 * Date: 08 Feb 2018
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

var glSection_Type = "0D";
var glAnchors = [];
// var glLocators = [];
// var glNFCdevices = [];
// var glRFIDdevices = [];
var glAreas = [];

var gl_anchor_num = 0;
var gl_locator_num = 0;

var gl_anchor_load_index = 0;
var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var glDropdown_checkbox_anchor_click = false;

var glSection_AlertTypes = [];
glSection_AlertTypes[0] = {};
glSection_AlertTypes[0].itemid = 0;
glSection_AlertTypes[0].itemname = "normal";
glSection_AlertTypes[1] = {};
glSection_AlertTypes[1].itemid = 1;
glSection_AlertTypes[1].itemname = "room";
glSection_AlertTypes[2] = {};
glSection_AlertTypes[2].itemid = 2;
glSection_AlertTypes[2].itemname = "danger";


var glSection_Types = [];
glSection_Types[0] = {};
glSection_Types[0].itemid = "0D";
glSection_Types[0].itemname = "0D";
glSection_Types[1] = {};
glSection_Types[1].itemid = "1D";
glSection_Types[1].itemname = "1D";

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
                load_areas();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_section_type();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_devices();
                //load_anchors();
                //load_locators();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_section_exist();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_alert_type();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                if (glSection_Type == "0D")
                {
                    document.getElementById('section_type_0D_btn-group').style.display = "block";
                    document.getElementById('section_type_1D_btn-group').style.display = "none";
                }
                else
                {
                    document.getElementById('section_type_0D_btn-group').style.display = "none";
                    document.getElementById('section_type_1D_btn-group').style.display = "block";
                }

                PreLoadIndex = CurrentLoadIndex;
                CurrentLoadIndex = 6;
            }
        break;
    }

    if ( CurrentLoadIndex != 6 )
        setTimeout(check_loading_status, 100);
}



function section_type_select(eventidx, itemidx)
{
    var select_value = $("#section_type_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_type_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_type_"+eventidx+"_name").html(select_value);
    $("#section_type_"+eventidx+"_name").attr("data-id", select_id);

    glSection_Type = select_value;

    if (select_id == "0D")
    {
        document.getElementById('section_type_0D_btn-group').style.display = "block";
        document.getElementById('section_type_1D_btn-group').style.display = "none";
    }
    else
    {
        document.getElementById('section_type_0D_btn-group').style.display = "none";
        document.getElementById('section_type_1D_btn-group').style.display = "block";
    }
}

function section_anchor_select(eventidx, itemidx)
{
    var select_value = $("#section_anchor_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_anchor_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_anchor_"+eventidx+"_name").html(select_value);
    $("#section_anchor_"+eventidx+"_name").attr("data-id", select_id);
}

function section_anchor_1_select(eventidx, itemidx)
{
    var select_value = $("#section_anchor_1_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_anchor_1_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_anchor_1_"+eventidx+"_name").html(select_value);
    $("#section_anchor_1_"+eventidx+"_name").attr("data-id", select_id);

    change_section_frompoint_value(select_value);
}

function section_anchor_2_select(eventidx, itemidx)
{
    var select_value = $("#section_anchor_2_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_anchor_2_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_anchor_2_"+eventidx+"_name").html(select_value);
    $("#section_anchor_2_"+eventidx+"_name").attr("data-id", select_id);

    change_section_topoint_value(select_value);
}

function section_area_select(eventidx, itemidx)
{
    var select_value = $("#section_area_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_area_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_area_"+eventidx+"_name").html(select_value);
    $("#section_area_"+eventidx+"_name").attr("data-id", select_id);
}

function section_alerttype_select(eventidx, itemidx)
{
    var select_alertvalue = $("#section_alerttype_"+eventidx+"_item_"+itemidx).html();
    var select_alertid = $("#section_alerttype_"+eventidx+"_item_"+itemidx).attr("data-id");

    $("#section_alerttype_"+eventidx+"_name").html(select_alertvalue);
    $("#section_alerttype_"+eventidx+"_name").attr("data-id", itemidx);
}

function section_type_delete(group_id)
{
    var retVal = confirm("Do you want to delete this section group?");
    if( retVal == true )
    {
        // remove UI button group
        // $("#event_trig_"+event_id).remove();
        // $("#event_trig_delete_"+event_id).remove();
        var SendData = "group_id="+group_id;
        SendData += "&project_id="+project_id;
        SendData += "&delete_group=1";

         return jQuery.ajax({
          url: '/php/sections.php',
          type: 'POST',
          data: SendData,
          dataType: 'text',
          // return jQuery.ajax({
          //  url: '/php/sections.php?group_id='+group_id+'&project_id='+project_id,
          // type: 'delete',
          // contentType: "application/json",
          // dataType: "json",
          // data: {},
          success:
          	function(response)
          	{
                //alert("event_trig_delete success, event_id="+event_id+",result="+result.eventid);
                console.log(JSON.stringify(response));
                setTimeout(load_section_exist, 100);
    		},
          error:
          	function(xhr, status, error)
          	{
                console.log(error);
                console.log(xhr.responseText);
                setTimeout(load_section_exist, 100);
    		}
        });
    }
    else
    {
        return false;
    }
}

function section_group_create()
{
    var section_type = $("#section_type_0_name").attr("data-id");

    //console.log("section_group_create() section_type:"+section_type);

    var json = {};
    if (section_type == "0D")
    {
        var section_0D_anchor = $("#section_anchor_0_name").attr("data-id");
        var section_0D_area = $("#section_area_0_name").attr("data-id");
        var section_0D_range = document.getElementById('section_range_selection_input').value;
        var section_0D_alerttype = $("#section_alerttype_0_name").attr("data-id");

        json['section_type'] = 0;
        json['project_id'] = project_id;
        json['anchor'] = section_0D_anchor;
        json['area'] = section_0D_area;
        json['range'] = section_0D_range;
        json['alerttype'] = section_0D_alerttype;

        console.log("section_group_create() section_0D_anchor:"+section_0D_anchor);
        console.log("section_group_create() section_0D_area:"+section_0D_area);
        console.log("section_group_create() section_0D_range:"+section_0D_range);
        console.log("section_group_create() section_0D_alerttype:"+section_0D_alerttype);
    }
    else
    if (section_type == "1D")
    {
        //var section_1D_anchor_1 = $("#section_anchor_1_0_name").attr("data-id");
        //var section_1D_anchor_2 = $("#section_anchor_2_0_name").attr("data-id");
        var section_1D_from = document.getElementById('section_frompoint_selection_input');
        var section_1D_to = document.getElementById('section_topoint_selection_input');

        if (section_1D_from != undefined && section_1D_to != undefined)
        {
            var section_1D_frompoint = section_1D_from.value;
            var section_1D_topoint = section_1D_to.value;

            var SectionAnchors = ""
            var index;
            for	(index = 0; index < glAnchors.length; index++)
            {
                var dropdown_checkbox_anchor = document.getElementById("dropdown_checkbox_anchor_"+index);
                if (dropdown_checkbox_anchor == undefined)
                {
                    continue;
                }

                if ( dropdown_checkbox_anchor.checked )
                {
                    if (SectionAnchors.length == 0)
                        SectionAnchors = glAnchors[index].itemid;
                    else
                        SectionAnchors += ","+glAnchors[index].itemid;
                }
            }

            json['project_id'] = ''+project_id;
            json['section_type'] = 1;
            //json['anchor1'] = section_1D_anchor_1;
            //json['anchor2'] = section_1D_anchor_2;
            json['anchors'] = SectionAnchors;
            json['frompoint'] = section_1D_frompoint;
            json['topoint'] = section_1D_topoint;

            //console.log("section_group_create() section_1D_anchor_1:"+section_1D_anchor_1);
            //console.log("section_group_create() section_1D_anchor_2:"+section_1D_anchor_2);
            console.log("section_group_create() SectionAnchors:"+SectionAnchors);
            console.log("section_group_create() section_1D_frompoint:"+section_1D_frompoint);
            console.log("section_group_create() section_1D_topoint:"+section_1D_topoint);
        }
    }


    return jQuery.ajax({
    url: '/php/sections.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
    	function(response)
    	{
            console.log(JSON.stringify(response));

            setTimeout(load_section_exist, 100);
            //window.location =  "/index.php/sections/"+project_id;
        },
    error:
    	function(xhr, status, error)
    	{
    	   console.log(error);
           console.log(xhr.responseText);
        }
    });

}

//
// Section 0D
//
function add_exist_section_0D(GroupID, SectionType, nodeIDList, AreaID, Param1, Param2)
{
    var section_box_body= document.getElementById('section_box_body');

    var section_breadcrumb = document.createElement('div');
    section_box_body.appendChild(section_breadcrumb);
    section_breadcrumb.setAttribute("class","breadcrumb section0");
    section_breadcrumb.id = "section_type_"+GroupID;

    // Section Type
    var btn_SectionType = document.createElement('a');
    section_breadcrumb.appendChild(btn_SectionType);
    btn_SectionType.setAttribute("class","active");
    // btn_SectionType.textContent = "0D";

    var span_SectionType = document.createElement("span");
    span_SectionType.textContent = "0D";
    span_SectionType.setAttribute("class","badge bg-gray disabled");
    btn_SectionType.appendChild(span_SectionType);

    // anchor
    var btn_anchor = document.createElement('a');
    section_breadcrumb.appendChild(btn_anchor);
    var btn_anchor_text = document.createTextNode(nodeIDList);
    btn_anchor.appendChild(btn_anchor_text);

    // Area name
    var btn_area = document.createElement('a');
    section_breadcrumb.appendChild(btn_area);
    var btn_area_text = document.createTextNode(AreaID);
    btn_area.appendChild(btn_area_text);

    // Range
    var btn_range = document.createElement('a');
    section_breadcrumb.appendChild(btn_range);
    var btn_range_text = document.createTextNode(Param1);
    btn_range.appendChild(btn_range_text);

    // alert type
    var btn_alerttype = document.createElement('a');
    section_breadcrumb.appendChild(btn_alerttype);
    var btn_alerttype_text = document.createTextNode(glSection_AlertTypes[parseInt(Param2)].itemname);
    //var btn_alerttype_text = document.createTextNode(Param2);
    btn_alerttype.appendChild(btn_alerttype_text);

    //
    // editor buttons
    //
    var btn_group_tools = document.createElement('div');
    btn_group_tools.setAttribute("class","btn-group pull-right");
    section_box_body.appendChild(btn_group_tools);

    var btn_tools = document.createElement('div');
    btn_tools.setAttribute("class","tools");
    btn_group_tools.appendChild(btn_tools);

    // delete button
    var btn_tools_delete = document.createElement('a');
    btn_tools.appendChild(btn_tools_delete);
    btn_tools_delete.setAttribute("href", "javascript:;");
    btn_tools_delete.setAttribute("onclick", "section_type_delete("+GroupID+");return false;");
    btn_tools_delete.setAttribute("data-id", "sectionid");
    btn_tools_delete.id = "section_type_delete_"+GroupID;
    var btn_tools_delete_img = document.createElement('i');
    btn_tools_delete_img.setAttribute("class", "fa fa-trash-o");
    btn_tools_delete.appendChild(btn_tools_delete_img);

    var br1 = document.createElement("br");
    section_box_body.appendChild(br1);
}

//
// Section 1D
//
function add_exist_section_1D(GroupID, SectionType, nodeIDList, Param1, Param2, Param3, Param4, Param5, Param6)
{
    var section_box_body= document.getElementById('section_box_body');

    var section_breadcrumb = document.createElement('div');
    section_box_body.appendChild(section_breadcrumb);
    section_breadcrumb.setAttribute("class","breadcrumb section1");
    section_breadcrumb.id = "section_type_"+GroupID;

    // Section Type
    var btn_SectionType = document.createElement('a');
    section_breadcrumb.appendChild(btn_SectionType);
    btn_SectionType.setAttribute("class","active");
    // btn_SectionType.textContent = "1D";

    var span_SectionType = document.createElement("span");
    span_SectionType.textContent = "1D";
    span_SectionType.setAttribute("class","badge bg-red disabled");
    btn_SectionType.appendChild(span_SectionType);

    // anchor
    var btn_anchor = document.createElement('a');
    section_breadcrumb.appendChild(btn_anchor);
    //==================
    // var btn_anchor_text = document.createTextNode(nodeIDList);
    // btn_anchor.appendChild(btn_anchor_text);
    //==================
    // var span_anchor = document.createElement("span");
    // span_anchor.textContent = nodeIDList;
    // span_anchor.setAttribute("class","badge bg-green-active");
    // btn_anchor.appendChild(span_anchor);
    //==================

    var anchorIDList = nodeIDList.split(',');
    for (var i = 0; i < anchorIDList.length; i++)
    {
        var anchor = anchorIDList[i];
        var span_anchor = document.createElement("span");
        span_anchor.textContent = anchor;
        span_anchor.setAttribute("class","badge bg-green-active");
        btn_anchor.appendChild(span_anchor);
    }

    // range start
    var btn_range_start = document.createElement('a');
    section_breadcrumb.appendChild(btn_range_start);
    var btn_range_start_text = document.createTextNode("("+Param1+","+Param2+","+Param3+")");
    btn_range_start.appendChild(btn_range_start_text);

    // range end
    var btn_range_end = document.createElement('a');
    section_breadcrumb.appendChild(btn_range_end);
    var btn_range_end_text = document.createTextNode("("+Param4+","+Param5+","+Param6+")");
    btn_range_end.appendChild(btn_range_end_text);

    //
    // editor buttons
    //
    var btn_group_tools = document.createElement('div');
    btn_group_tools.setAttribute("class","btn-group pull-right");
    section_box_body.appendChild(btn_group_tools);

    var btn_tools = document.createElement('div');
    btn_tools.setAttribute("class","tools");
    btn_group_tools.appendChild(btn_tools);

    // delete button
    var btn_tools_delete = document.createElement('a');
    btn_tools.appendChild(btn_tools_delete);
    btn_tools_delete.setAttribute("href", "javascript:;");
    btn_tools_delete.setAttribute("onclick", "section_type_delete("+GroupID+");return false;");
    btn_tools_delete.id = "section_type_delete_"+GroupID;
    var btn_tools_delete_img = document.createElement('i');
    btn_tools_delete_img.setAttribute("class", "fa fa-trash-o");
    btn_tools_delete.appendChild(btn_tools_delete_img);

    var br1 = document.createElement("br");
    section_box_body.appendChild(br1);

}

function load_section_exist()
{
    var targetURL = gl_target_server + "/php/sections.php";
    $.getJSON(targetURL, 'loadsections=1'+'&project_id='+project_id, function(data)
    {
        var section_box_body= document.getElementById('section_box_body');
        while (section_box_body.firstChild)
        {
            section_box_body.removeChild(section_box_body.firstChild);
        }


        var index = 1;
        var sections = [];
        $.each(data, function(sectionkey, value)
        {
            //=====================
            //var anchors = value.nodeIDList;
            //=====================
            var anchors = [];
            var nodeIDList = value.nodeIDList.split(',');
            for (var i = 0; i < nodeIDList.length; i++)
            {
                var node = nodeIDList[i];
                var found = false;
                for (var j = 0; j < glAnchors.length; j++)
                {
                    var anchor = glAnchors[j];

                    // console.log("glAnchors.length:"+glAnchors.length+" j:"+j+" anchor:"+anchor);

                    if (anchor != undefined && anchor.itemid == node)
                    {
                        if (anchors.length == 0)
                            anchors = anchor.itemname;
                        else
                            anchors = anchors + "," + anchor.itemname;
                        found = true;
                        break;
                    }
                }
                if (found == false)
                {
                    if (anchors.length == 0)
                        anchors = node;
                    else
                        anchors = anchors + "," + node;
                }
            }
            //=====================

            var areaname = "";
            for (var i = 0; i < glAreas.length; i++)
            {
                var area = glAreas[i];
                if (area.itemid == value.AreaID)
                {
                    areaname = area.itemname;
                    break;
                }
            }

            if ( value.SectionType == "0" )
            {
                add_exist_section_0D(parseInt(sectionkey), value.SectionType, anchors, areaname, value.Param1, value.Param2);
            }
            else
            {
                add_exist_section_1D(parseInt(sectionkey), value.SectionType, anchors,
                    value.Param1, value.Param2, value.Param3, value.Param4, value.Param5, value.Param6);
            }

            index++;
        });

        CurrentLoadIndex = 4;
    });
    return false;
}

function load_Items_to_newevent_UI(itemtype, uitype, tag_selection_btn_group, items)
{
    //var tags = [];
    //var index = 0;
    var itemid = "";
    var itemname = "";

    console.log("load_Items_to_newevent_UI() items.length:"+items.length +" itemtype:"+itemtype);

    if ( items.length > 0 )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = "section_"+itemtype+"_0_name";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    btn_group1_button2.id = "section_"+itemtype+"_0";

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
        if ( items[i] == undefined )
        {
            continue;
        }

        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "section_"+itemtype+"_select(0,"+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = "section_"+itemtype+"_0_item_"+i;
        //
        // console.log("load_Items_to_newevent_UI() itemtype:"+ itemtype +" items[i].type :"+items[i].type +" items[i].itemname :"+items[i].itemname );

        if (itemtype == "anchor" && items[i].type == "anchor")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname;
            span_anchor.setAttribute("class","badge bg-green-active");
            dropdown_li_0_href.appendChild(span_anchor);
        }
        else
        if (itemtype == "anchor" && items[i].type == "locator")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname;
            span_anchor.setAttribute("class","badge bg-yellow-active");
            dropdown_li_0_href.appendChild(span_anchor);
        }
        else
        if (itemtype == "anchor" && items[i].type == "rfiddevice")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname + " (RFID)";
            span_anchor.setAttribute("class","badge bg-aqua-active");
            dropdown_li_0_href.appendChild(span_anchor);
        }
        else
        if (itemtype == "anchor" && items[i].type == "nfcdevice")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname + " (NFC)";
            span_anchor.setAttribute("class","badge bg-yellow");
            dropdown_li_0_href.appendChild(span_anchor);
        }
        else
        if (items[i].itemid == "0D")
        {
            var span_SectionType = document.createElement("span");
            span_SectionType.textContent = items[i].itemname;
            span_SectionType.setAttribute("class","badge bg-gray disabled");
            dropdown_li_0_href.appendChild(span_SectionType);
        }
        else
        if (items[i].itemid == "1D")
        {
            var span_SectionType = document.createElement("span");
            span_SectionType.textContent = items[i].itemname;
            span_SectionType.setAttribute("class","badge bg-red disabled");
            dropdown_li_0_href.appendChild(span_SectionType);
        }
        else
        {
            var dropdown_li_0_href_text = document.createTextNode(items[i].itemname);
            dropdown_li_0_href.appendChild(dropdown_li_0_href_text);
        }
    }

}

function load_dropdown_Items_to_UI(dropdown_btn_group, anchors)
{
    //console.log("load_dropdown_Items_to_UI() anchors.length:"+ anchors.length);
    var index;
    for	(index = 0; index < anchors.length; index++)
    {
        if ( anchors[index] == undefined )
        {
            continue;
        }

        var itemid = anchors[index].itemid;
        var itemname = anchors[index].itemname;
        var defaultchecked = false;
        // if (index == 0 )
        // {
        //     defaultchecked = true;
        //     document.getElementById("dropdown_cur_anchor").innerHTML = itemname + '<span class="caret"></span>';
        // }

        console.log("load_dropdown_Items_to_UI() itemid:"+ itemid +" itemname:"+ itemname );

        var item_li = create_dropdown_anchor_item(index, itemid, itemname, defaultchecked);
        dropdown_btn_group.appendChild(item_li);
    }
}

function load_section_type()
{
    var section_type_selection_btn_group = document.getElementById('section_type_selection');

    load_Items_to_newevent_UI("type", "btn-warning", section_type_selection_btn_group, glSection_Types);

    CurrentLoadIndex = 2;
}

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        console.log("load_devices() gl_anchor_num:"+gl_anchor_num+", gl_locator_num:"+gl_locator_num);

        gl_anchor_load_index = 0;
        gl_locator_load_index = 0;
    })
    .success(function() {
        setTimeout(load_anchors, 10);
    })
    .error(function() {
        setTimeout(load_devices, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_anchors()
{

    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(load_locators, 10);
        return;
    }
    if (gl_anchor_load_index == 0)
    {
        glAnchors = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        console.log("load_anchors() anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            console.log("load_anchors() key:"+key +", data[key]:"+data[key]);

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glAnchors[i] = {};
            glAnchors[i].itemid = nodeData[3];
            glAnchors[i].itemname = nodeData[5];
            glAnchors[i].posX = nodeData[7];
            glAnchors[i].posY = nodeData[8];
            glAnchors[i].posZ = nodeData[9];
            glAnchors[i].type = "anchor";
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {

        console.log("load_anchors() success gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            setTimeout(load_locators, 10);
        }
        else
        {
            load_anchors();
        }
    })
    .error(function() {

        console.log("load_anchors() error gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        gl_anchor_load_index = 0;
        setTimeout(load_locators, 10);
    })
    .complete(function() {
    });
}

function load_locators()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        load_RFIDdevices();
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        console.log("load_locators() locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

        var anchor_size = gl_anchor_num;

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glAnchors[anchor_size + i] = {};
            glAnchors[anchor_size + i].itemid = nodeData[3];
            glAnchors[anchor_size + i].itemname = nodeData[5];
            glAnchors[anchor_size + i].posX = nodeData[7];
            glAnchors[anchor_size + i].posY = nodeData[8];
            glAnchors[anchor_size + i].posZ = nodeData[9];
            glAnchors[anchor_size + i].type = "locator";
        }

        gl_locator_load_index += locator_num;
        gl_anchor_num += locator_num;

    })
    .success(function() {

        console.log("load_locators() gl_locator_load_index:"+gl_locator_load_index +", gl_locator_num:"+gl_locator_num);

        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;

            load_RFIDdevices();
        }
        else
        {
            load_locators();
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        setTimeout(load_locators, 1000);
    })
    .complete(function() {
    });
}

function load_RFIDdevices()
{
    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&s=0&count=100&project_id='+project_id+'&showonline=1&showoffline=1', function(data)
    {
        var anchor_size = gl_anchor_num;

        $.each(data, function(index, device)
        {
            glAnchors[anchor_size] = {};
            glAnchors[anchor_size].itemid = device.macaddress;
            glAnchors[anchor_size].itemname = device.name;
            glAnchors[anchor_size].posX = 0;
            glAnchors[anchor_size].posY = 0;
            glAnchors[anchor_size].posZ = 0;
            glAnchors[anchor_size].type = "rfiddevice";
            anchor_size += 1;
        });

        gl_anchor_num += anchor_size;
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        load_NFCdevices();
    });
}

function load_NFCdevices()
{
    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&s=0&count=100&project_id='+project_id+'&showonline=1&showoffline=1', function(data)
    {
        var anchor_size = gl_anchor_num;

        $.each(data, function(index, device)
        {
            glAnchors[anchor_size] = {};
            glAnchors[anchor_size].itemid = device.macaddress;
            glAnchors[anchor_size].itemname = device.name;
            glAnchors[anchor_size].posX = 0;
            glAnchors[anchor_size].posY = 0;
            glAnchors[anchor_size].posZ = 0;
            glAnchors[anchor_size].type = "nfcdevice";
            anchor_size += 1;
        });

        gl_anchor_num += anchor_size;

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        show_anchor_to_UI();
        CurrentLoadIndex = 3;
    });
}

function show_anchor_to_UI()
{
    // 0D
    var section_anchor_selection_btn_group = document.getElementById('section_anchor_selection');
    load_Items_to_newevent_UI("anchor", "btn-info", section_anchor_selection_btn_group, glAnchors);

    // 1D
    // var section_anchor_selection_1_btn_group = document.getElementById('section_anchor_selection_1');
    // var section_anchor_selection_2_btn_group = document.getElementById('section_anchor_selection_2');
    // load_Items_to_newevent_UI("anchor_1", "btn-info", section_anchor_selection_1_btn_group, glAnchors);
    // load_Items_to_newevent_UI("anchor_2", "btn-info", section_anchor_selection_2_btn_group, glAnchors);

    // 1D
    var dropdown_anchors_btn_group = document.getElementById('dropdown_anchors');
    load_dropdown_Items_to_UI(dropdown_anchors_btn_group, glAnchors);
}

function load_locators_old()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";

    //console.log("load_anchors() targetURL:"+targetURL);

    $.getJSON(targetURL, '&loadlocator=1'+'&project_id='+project_id, function(data)
    {
        var loc_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        var anchor_size = glAnchors.length;
        console.log("load_locators() loc_locator_num:"+loc_locator_num +", anchor_size:"+anchor_size);

        for (var i = 0; i < loc_locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            var node = data[key];
            var nodeData = node.split(",");

            glAnchors[anchor_size + i] = {};
            glAnchors[anchor_size + i].itemid = nodeData[3];
            glAnchors[anchor_size + i].itemname = nodeData[5];
            glAnchors[anchor_size + i].posX = nodeData[7];
            glAnchors[anchor_size + i].posY = nodeData[8];
            glAnchors[anchor_size + i].posZ = nodeData[9];
            glAnchors[anchor_size + i].type = "locator";
        }

        // 0D
        var section_anchor_selection_btn_group = document.getElementById('section_anchor_selection');
        load_Items_to_newevent_UI("anchor", "btn-info", section_anchor_selection_btn_group, glAnchors);

        // 1D
        // var section_anchor_selection_1_btn_group = document.getElementById('section_anchor_selection_1');
        // var section_anchor_selection_2_btn_group = document.getElementById('section_anchor_selection_2');
        // load_Items_to_newevent_UI("anchor_1", "btn-info", section_anchor_selection_1_btn_group, glAnchors);
        // load_Items_to_newevent_UI("anchor_2", "btn-info", section_anchor_selection_2_btn_group, glAnchors);

        // 1D
        var dropdown_anchors_btn_group = document.getElementById('dropdown_anchors');
        load_dropdown_Items_to_UI(dropdown_anchors_btn_group, glAnchors);

        CurrentLoadIndex = 3;
    });
}

function load_areas()
{
    var section_area_selection_btn_group = document.getElementById('section_area_selection');

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'project_id='+project_id, function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, area)
        {
            var area_json = JSON.parse(area);
            //alert('itemindex='+itemindex+", area_json.properties.areaid="+area_json.properties.areaid);
            glAreas[index] = {};
            glAreas[index].itemid = area_json.properties.areaid;
            glAreas[index].itemname = area_json.properties.areaname;

            //alert('itemindex='+itemindex+", area.areaid="+area_json['areaid']);

            index++;
        });

        load_Items_to_newevent_UI("area", "btn-success", section_area_selection_btn_group, glAreas);

        if (glSection_Type == "1D")
        {
            section_area_selection_btn_group.style.visibility = "hidden";
            document.getElementById('section_area_selection_text').style.visibility = "hidden";
        }

        CurrentLoadIndex = 1;
    });
}

function load_alert_type()
{
    var section_alerttype_selection_btn_group = document.getElementById('section_alerttype_selection');

    load_Items_to_newevent_UI("alerttype", "btn-success", section_alerttype_selection_btn_group, glSection_AlertTypes);

    CurrentLoadIndex = 5;
}

function load_section_conditions()
{
    load_section_type();
    load_anchors();
    load_locators();
    load_areas();
    load_alert_type();

    if (glSection_Type == "0D")
    {
        document.getElementById('section_type_0D_btn-group').style.display = "block";
        document.getElementById('section_type_1D_btn-group').style.display = "none";
    }
    else
    {
        document.getElementById('section_type_0D_btn-group').style.display = "none";
        document.getElementById('section_type_1D_btn-group').style.display = "block";
    }
    return false;
}


function change_section_frompoint_value(anchor_id)
{
    //console.log("change_section_frompoint_value() anchor_id:"+anchor_id);
    for (var i = 0; i < glAnchors.length; i++)
    {
        anchor = glAnchors[i];
        //console.log("anchor.itemid:"+anchor.itemid);
        if (anchor.itemid == anchor_id)
        {
            document.getElementById('section_frompoint_selection_input').value = anchor.posX+","+anchor.posY+","+anchor.posZ;
            break;
        }
    }
}

function change_section_topoint_value(anchor_id)
{
    for (var i = 0; i < glAnchors.length; i++)
    {
        anchor = glAnchors[i];
        if (anchor.itemid == anchor_id)
        {
            document.getElementById('section_topoint_selection_input').value = anchor.posX+","+anchor.posY+","+anchor.posZ;
            break;
        }
    }
}


function create_dropdown_anchor_item(anchorindex, nodeid, nodename, defaultchecked)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", showtext:"+showtext);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", anchorindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_anchor_click(\""+ anchorindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_anchor_"+anchorindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_anchor_click(\""+ anchorindex +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    //======================
    var span_anchor = document.createElement("span");
    span_anchor.textContent = nodename;
    span_anchor.setAttribute("class","badge bg-green-active");
    item_click.appendChild(span_anchor);
    //======================
    //item_click.appendChild(document.createTextNode(" "+nodeid));
    //======================
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_anchor_click(anchorindex)
{
    glDropdown_checkbox_anchor_click = true;
}

var glDropdown_checkbox_anchor_click = false;
function dropdown_anchor_click(anchorindex)
{
    // console.log("dropdown_anchor_click() anchorindex:"+anchorindex+", glDropdown_checkbox_anchor_click:"+glDropdown_checkbox_anchor_click);
    // console.log("dropdown_anchor_click() checked:"+document.getElementById("dropdown_checkbox_anchor_"+anchorindex).checked);

    if(glDropdown_checkbox_anchor_click)
    {
        glDropdown_checkbox_anchor_click = false;
        return;
    }

    if ( document.getElementById("dropdown_checkbox_anchor_"+anchorindex).checked )
        document.getElementById("dropdown_checkbox_anchor_"+anchorindex).checked = false;
    else
        document.getElementById("dropdown_checkbox_anchor_"+anchorindex).checked = true;

    glDropdown_checkbox_anchor_click = false;
}

//add_exist_section(1, "tag1_id", "tag1", "enter", "enter", "areaid", "areaname", "issuetime", "Immediately");
//add_exist_event(2, "tag2_id", "tag2", "Leave", "enter", "areaid", "areaname", "issuetime", "Immediately");
//load_section_conditions();
//load_section_exist();


function open_sections_page()
{
    var win = window.open('/index.php/vilssections/'+project_id, '_blank');
    win.focus();
}
