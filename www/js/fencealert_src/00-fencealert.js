/*
 * Author: ikki Chung
 * Date: 2020/03/09
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

var glAnchors = [];
var glLocators = [];

var gl_anchor_num = 0;
var gl_locator_num = 0;

var gl_anchor_load_index = 0;
var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var glDropdown_checkbox_anchor_click = false;

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

                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_fencealert_exist();
                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                //load_alert_type();
                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
    }

    if ( CurrentLoadIndex != 6 )
        setTimeout(check_loading_status, 100);
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
    //setTimeout(function(){ p.abort(); }, 5000);

}

function load_locators()
{

    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;

        show_anchor_to_UI()

        return;
    }
    if (gl_locator_load_index == 0)
    {
        glLocator_List = [];
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

    })
    .success(function() {

        console.log("load_locators() gl_locator_load_index:"+gl_locator_load_index +", gl_locator_num:"+gl_locator_num);

        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;

            show_anchor_to_UI();

            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            load_locators();
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
        setTimeout(load_locators, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function show_anchor_to_UI()
{
    var fencealert_anchor_selection_btn_group = document.getElementById('fencealert_anchor_selection');
    load_Items_to_newevent_UI("anchor", "btn-info", fencealert_anchor_selection_btn_group, glAnchors);
}

function load_Items_to_newevent_UI(itemtype, uitype, tag_selection_btn_group, items)
{
    //var tags = [];
    //var index = 0;
    var itemid = "";
    var itemname = "";

    console.log("load_Items_to_newevent_UI() items.length:"+items.length);

    if ( items.length > 0 )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = "fencealert_"+itemtype+"_name";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    btn_group1_button2.id = "fencealert_"+itemtype;

    var btn_group1_button2_span1 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span1);
    btn_group1_button2_span1.setAttribute("class","caret");

    var btn_group1_button2_span2 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span2);
    btn_group1_button2_span2.setAttribute("class","sr-only");

    var dropdown_ul = document.createElement('ul');
    tag_selection_btn_group.appendChild(dropdown_ul);
    dropdown_ul.setAttribute("class", "dropdown-menu");
    dropdown_ul.setAttribute("role", "menu");

    for (var i = 0; i < items.length; i++)
    {
        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "fencealert_"+itemtype+"_select("+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = "fencealert_"+itemtype+"_item_"+i;
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
        {
            var dropdown_li_0_href_text = document.createTextNode(items[i].itemname);
            dropdown_li_0_href.appendChild(dropdown_li_0_href_text);
        }
    }
}

function fencealert_anchor_select(itemidx)
{
    var select_value = $("#fencealert_anchor_item_"+itemidx).html();
    var select_id = $("#fencealert_anchor_item_"+itemidx).attr("data-id");

    $("#fencealert_anchor_name").html(select_value);
    $("#fencealert_anchor_name").attr("data-id", select_id);
}

function load_fencealert_exist()
{
    var targetURL = gl_target_server + "/php/fencealert.php";
    $.getJSON(targetURL, 'loadfencealert=1'+'&project_id='+project_id, function(data)
    {
        var fencealert_box_body= document.getElementById('fencealert_box_body');
        while (fencealert_box_body.firstChild)
        {
            fencealert_box_body.removeChild(fencealert_box_body.firstChild);
        }


        var index = 1;
        var sections = [];
        $.each(data, function(sectionkey, value)
        {
            add_exist_fencealert(parseInt(sectionkey), value.nodeid,value.distance);

            index++;
        });

        CurrentLoadIndex = CurrentLoadIndex + 1;
    });

    return false;
}

//
// Section 0D
//
function add_exist_fencealert(GroupID, anchor, distance)
{
    var fencealert_box_body= document.getElementById('fencealert_box_body');

    var fencealert_breadcrumb = document.createElement('div');
    fencealert_box_body.appendChild(fencealert_breadcrumb);
    fencealert_breadcrumb.setAttribute("class","breadcrumb section0");
    fencealert_breadcrumb.id = "fencealert_type_"+GroupID;

    // anchor
    var btn_anchor = document.createElement('a');
    fencealert_breadcrumb.appendChild(btn_anchor);
    var btn_anchor_text = document.createTextNode(anchor);
    btn_anchor.appendChild(btn_anchor_text);

    // Range
    var btn_range = document.createElement('a');
    fencealert_breadcrumb.appendChild(btn_range);
    var btn_range_text = document.createTextNode(distance);
    btn_range.appendChild(btn_range_text);

    //
    // editor buttons
    //
    var btn_group_tools = document.createElement('div');
    btn_group_tools.setAttribute("class","btn-group pull-right");
    fencealert_box_body.appendChild(btn_group_tools);

    var btn_tools = document.createElement('div');
    btn_tools.setAttribute("class","tools");
    btn_group_tools.appendChild(btn_tools);

    // delete button
    var btn_tools_delete = document.createElement('a');
    btn_tools.appendChild(btn_tools_delete);
    btn_tools_delete.setAttribute("href", "javascript:;");
    btn_tools_delete.setAttribute("onclick", "fencealert_type_delete("+GroupID+");return false;");
    btn_tools_delete.setAttribute("data-id", "sectionid");
    btn_tools_delete.id = "fencealert_type_delete_"+GroupID;
    var btn_tools_delete_img = document.createElement('i');
    btn_tools_delete_img.setAttribute("class", "fa fa-trash-o");
    btn_tools_delete.appendChild(btn_tools_delete_img);

    var br1 = document.createElement("br");
    fencealert_box_body.appendChild(br1);
}

function load_dropdown_Items_to_UI(dropdown_btn_group, anchors)
{
    //console.log("load_dropdown_Items_to_UI() anchors.length:"+ anchors.length);
    var index;
    for	(index = 0; index < anchors.length; index++)
    {
        var itemid = anchors[index].itemid;
        var itemname = anchors[index].itemname;
        var defaultchecked = false;

        var item_li = create_dropdown_anchor_item(index, itemid, defaultchecked);
        dropdown_btn_group.appendChild(item_li);
    }
}

function create_dropdown_anchor_item(anchorindex, nodeid, defaultchecked)
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
    span_anchor.textContent = nodeid;
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

function fencealert_type_delete(group_id)
{
    var retVal = confirm("Do you want to delete this fence alert group?");
    if( retVal == true )
    {
        var SendData = "group_id="+group_id;
        SendData += "&project_id="+project_id;
        SendData += "&delete_fencealertgroup=1";

        return jQuery.ajax({
         url: '/php/fencealert.php',
         type: 'POST',
         data: SendData,
         dataType: 'text',
         // return jQuery.ajax({
         //  url: '/php/fencealert.php?group_id='+group_id+'&project_id='+project_id,
         //  type: 'delete',
         //  contentType: "application/json",
         //  dataType: "json",
         //  data: {},
          success:
          	function(response)
          	{
                console.log(JSON.stringify(response));
                setTimeout(load_fencealert_exist, 1000);
    		},
          error:
          	function(xhr, status, error)
          	{
                console.log(error);
                console.log(xhr.responseText);
                setTimeout(load_fencealert_exist, 1000);
    		}
        });
    }
    else
    {
        return false;
    }
}

function fencealert_group_create()
{
    var json = {};

    var fencealert_anchor = $("#fencealert_anchor_name").attr("data-id");
    var fencealert_range = document.getElementById('fencealert_range_selection_input').value;

    json['project_id'] = project_id;
    json['anchor'] = fencealert_anchor;
    json['distance'] = fencealert_range;

    console.log("fencealert_group_create() fencealert_anchor:"+fencealert_anchor);
    console.log("fencealert_group_create() fencealert_range:"+fencealert_range);

    return jQuery.ajax({
    url: '/php/fencealert.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
    	function(response)
    	{
            console.log(JSON.stringify(response));

            setTimeout(load_fencealert_exist, 1000);
        },
    error:
    	function(xhr, status, error)
    	{
    	   console.log(error);
           console.log(xhr.responseText);
        }
    });
}

function open_fencealert_page()
{
    var win = window.open('/index.php/vilsfencealert/'+project_id, '_blank');
    win.focus();
}
