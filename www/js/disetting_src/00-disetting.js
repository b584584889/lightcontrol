/*
 * Author: ikki Chung
 * Date: 02 Sep 2020
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_usersubgroups = [];
var gl_SubgroupUIBlocks = [];

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
                load_usersubgroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_subgroup_ui_info();

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

function load_usersubgroup()
{
    gl_usersubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusersubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usersubgroup)
        {
            usersubgroup.subgroupid = parseInt(usersubgroup.subgroupid);
            gl_usersubgroups.push(usersubgroup);
        });
    })
    .success(function() {

        add_subgroup_to_table();
        add_subgroup_to_dropdown();
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function add_subgroup_to_table()
{
    var subgroup_list_content = document.getElementById('subgroup_list_content');

    while (subgroup_list_content.rows.length >= 1)
    {
        subgroup_list_content.deleteRow(0);
    }

    // console.log("add_subgroup_to_table() gl_usersubgroups.length:"+gl_usersubgroups.length);

    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];
        subgroup_list_content.appendChild( create_subgroup_item(i+1, subgroup) );
    }
}

function create_subgroup_item(index, subgroup)
{
    var subgroup_list_content = document.getElementById('subgroup_list_content');

    var table_tr = document.createElement('tr');
    subgroup_list_content.appendChild(table_tr);

    var table_td_1 = document.createElement('td');
    table_td_1.textContent = index;
    table_td_1.setAttribute("id","subgroup_row_"+ subgroup.subgroupid);
    table_tr.appendChild(table_td_1);

    var table_td_checkbox = document.createElement('td');
    table_tr.appendChild(table_td_checkbox);

    var ui_checkbox = document.createElement('input');
    ui_checkbox.setAttribute("id","subgroup_row_checkbox_"+ subgroup.subgroupid);
    ui_checkbox.setAttribute("type",'checkbox');
    ui_checkbox.setAttribute("onclick","click_subgroup(\""+ subgroup.subgroupid +"\");return true;");
    table_td_checkbox.appendChild(ui_checkbox);

    var table_td_subgroupname = document.createElement('td');
    table_td_subgroupname.setAttribute("id","subgroup_row_name_"+ subgroup.subgroupid);
    table_tr.appendChild(table_td_subgroupname);

    // node name
    var subgroupname_span = create_subgroup_label(subgroup.subgroupname);
    subgroupname_span.setAttribute("onclick","checkbox_subgroup_click(\""+ subgroup.subgroupid +"\");return true;");
    table_td_subgroupname.appendChild(subgroupname_span);

    // var rolename_span = document.createElement('span');
    // rolename_span.setAttribute("class",'badge bg-green');
    // rolename_span.textContent = subgroup.subgroupname;
    // rolename_span.setAttribute("onclick","checkbox_subgroup_click(\""+ subgroup.subgroupid +"\");return true;");
    // table_td_rolename.appendChild(rolename_span);

    return table_tr;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function click_subgroup(subgroupid)
{
    update_uiblock_list();
}

function checkbox_subgroup_click(subgroupid)
{
    var checkbox_ui = document.getElementById("subgroup_row_checkbox_"+ subgroupid);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_subgroup_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_uiblock_list();
}

function subgroup_list_checkbox_click()
{
    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);

        id_row_checkbox.checked = document.getElementById("subgroup_list_checkbox").checked;
    }

    update_uiblock_list();
}

function add_subgroup_to_dropdown()
{
    var copy_from_uisetting_dropdown_menu = document.getElementById('copy_from_uisetting_dropdown_menu');
    if (copy_from_uisetting_dropdown_menu != undefined)
    {
        while (copy_from_uisetting_dropdown_menu.firstChild)
        {
            copy_from_uisetting_dropdown_menu.removeChild(copy_from_uisetting_dropdown_menu.firstChild);
        }
    }

    var copy_to_uisetting_dropdown_menu = document.getElementById('copy_to_uisetting_dropdown_menu');
    while (copy_to_uisetting_dropdown_menu.firstChild)
    {
        copy_to_uisetting_dropdown_menu.removeChild(copy_to_uisetting_dropdown_menu.firstChild);
    }

    // console.log("add_subgroup_to_table() gl_usersubgroups.length:"+gl_usersubgroups.length);

    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];
        if (copy_from_uisetting_dropdown_menu != undefined)
        {
            copy_from_uisetting_dropdown_menu.appendChild( create_subgroup_dropdown_item('from', i+1, 1, subgroup) );
        }
        copy_to_uisetting_dropdown_menu.appendChild( create_subgroup_dropdown_item('to', i+1, 0, subgroup) );
    }

}

function create_subgroup_dropdown_item(prefix, index, type, subgroup)
{
    var dropdown_li = document.createElement('li');

    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", subgroup.subgroupid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_subgroup_click(\""+prefix+"\",\""+subgroup.subgroupid+"\","+index+");return true;");

    var item_checkbox = document.createElement("input");
    if (type == 0)
    {
        item_checkbox.setAttribute("type","checkbox");
    }
    else
    {
        item_checkbox.setAttribute("type","radio");
        item_checkbox.setAttribute("name","radio_"+prefix);
    }
    item_checkbox.setAttribute("id","dropdown_checkbox_subgroup_"+prefix+"_"+subgroup.subgroupid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_subgroup_click(\""+prefix+"\");return true;");
    item_checkbox.checked = false;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+subgroup.subgroupname));
    dropdown_li.appendChild(item_click);

    return dropdown_li;
}

var glDropdown_checkbox_subgroup_click = false;
function dropdown_subgroup_click(prefix, subgroupid, index)
{
    // console.log("dropdown_subgroup_click() prefix:"+prefix+" subgroupid:"+subgroupid+" index:"+index);

    if (glDropdown_checkbox_subgroup_click == true)
    {
        glDropdown_checkbox_subgroup_click = false;
        return;
    }

    var checkbox_ui = document.getElementById("dropdown_checkbox_subgroup_"+ prefix+"_"+ subgroupid+"_"+index);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_subgroup_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;


    check_copy_button(prefix);
}

function dropdown_checkbox_subgroup_click(prefix)
{
    glDropdown_checkbox_subgroup_click = true;

    check_copy_button(prefix);
}

function check_copy_button(prefix)
{
    var check_count = 0;
    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];
        var index = i + 1;

        var checkbox_ui = document.getElementById("dropdown_checkbox_subgroup_"+ prefix+"_"+ subgroup.subgroupid+"_"+index);
        if (checkbox_ui != undefined)
        {
            if ( checkbox_ui.checked)
            {
                check_count++;
                break;
            }
        }
    }
    var uisetting_button = document.getElementById("copy_"+prefix+"_uisetting_button");

    console.log("check_copy_button() copy_"+prefix+"_uisetting_button");

    if (check_count > 0)
    {
        uisetting_button.setAttribute("style","");
    }
    else
    {
        uisetting_button.setAttribute("style","visibility:hidden;");
    }
}

function copy_from_uisetting_button_click()
{
    var from_subgroupid = 0;
    var from_subgroupname = "";

    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];
        var index = i + 1;

        var checkbox_ui = document.getElementById("dropdown_checkbox_subgroup_from_"+ subgroup.subgroupid+"_"+index);
        if (checkbox_ui != undefined)
        {
            if ( checkbox_ui.checked)
            {
                from_subgroupname = subgroup.subgroupname;
                from_subgroupid = subgroup.subgroupid;
                break;
            }
        }
    }

    var subgroupids = [];
    var to_subgroupname = '';

    var check_count = 0;
    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);

        if ( id_row_checkbox.checked )
        {
            if (check_count > 0)
                to_subgroupname = to_subgroupname+',';
            to_subgroupname = to_subgroupname+ subgroup.subgroupname;

            subgroupids[check_count] = subgroup.subgroupid;
            check_count++;
        }
    }

    if (confirm('確定將子集合['+from_subgroupname+']設定 複製到目前子集合['+to_subgroupname+']設定?'))
    {
        var curUIBlocks = gl_SubgroupUIBlocks[from_subgroupid];

        for (var i = 0; i < subgroupids.length; i++)
        {
            var subgroupid = subgroupids[i];
            gl_SubgroupUIBlocks[subgroupid] = curUIBlocks;
        }

        var jsonstring = JSON.stringify(curUIBlocks);
        var str_subgroupids = subgroupids.toString();

        glUIBlocks = curUIBlocks;

        update_subgroupuiblock_record(str_subgroupids, jsonstring, true);
    }
    else
    {
        // Do nothing!
    }
}

function copy_to_uisetting_button_click()
{
    var subgroupids = [];
    var to_subgroupname = '';

    var check_count = 0;
    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];
        var index = i + 1;

        var checkbox_ui = document.getElementById("dropdown_checkbox_subgroup_to_"+ subgroup.subgroupid+"_"+index);
        if (checkbox_ui != undefined)
        {
            if ( checkbox_ui.checked)
            {
                if (check_count > 0)
                    to_subgroupname = to_subgroupname+',';
                to_subgroupname = to_subgroupname+ subgroup.subgroupname;

                subgroupids[check_count] = subgroup.subgroupid;
                check_count++;
            }
        }
    }

    if (confirm('確定將目前設定 複製到子集合['+to_subgroupname+']設定?'))
    {
        var jsonstring = JSON.stringify(glUIBlocks);
        var str_subgroupids = subgroupids.toString();

        for (var i = 0; i < subgroupids.length; i++)
        {
            var subgroupid = subgroupids[i];
            gl_SubgroupUIBlocks[subgroupid] = glUIBlocks;
        }

        update_subgroupuiblock_record(str_subgroupids, jsonstring, false);

    }
    else
    {
        // Do nothing!
    }
}

function update_uiblock_list()
{
    glUIBlocks = [];

    var select_count = 0;

    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);

        if ( id_row_checkbox.checked )
        {
            // console.log("update_uiblock_list() checked subgroup.subgroupid:"+subgroup.subgroupid +" select_count:"+select_count);

            //var int_subgroupid = parseInt(subgroup.subgroupid);

            var uiBlocks = gl_SubgroupUIBlocks[subgroup.subgroupid];
            if ( uiBlocks != undefined )
            {
                // console.log("update_uiblock_list() uiBlocks.length:"+uiBlocks.length);

                // 檢查重複
                for(var irow=0; irow<uiBlocks.length;irow++)
                {
                    var uirowdata = uiBlocks[irow];
                    for(var icol=0; icol<uirowdata.length;icol++)
                    {
                        var uiblock = uirowdata[icol];
                        var found = check_existed_block(uiblock);
                        if ( found == false )
                        {
                            // add
                            // console.log("update_uiblock_list() found == false uiblock.id:"+uiblock.id);

                            put_to_existed_block(irow, icol, uiblock);
                        }
                        // else
                        // {
                        //     console.log("update_uiblock_list() found == true uiblock.id:"+uiblock.id);
                        // }
                    }
                }
            }
            // else
            // {
            //     console.log("update_uiblock_list() uiBlocks == undefined ");
            // }

            select_count++;
        }
    }

    var save_uisetting_button = document.getElementById("save_uisetting_button");
    var restore_uisetting_button = document.getElementById("restore_uisetting_button");
    var copy_from_uisetting_dropdown = document.getElementById("copy_from_uisetting_dropdown");
    var copy_to_uisetting_dropdown = document.getElementById("copy_to_uisetting_dropdown");


    if (select_count > 0)
    {
        save_uisetting_button.setAttribute("style", "display:block;");
        restore_uisetting_button.setAttribute("style", "display:block;");
        copy_from_uisetting_dropdown.setAttribute("style", "display:block;");
        copy_to_uisetting_dropdown.setAttribute("style", "display:block;");

        if (glUIBlocks.length == 0)
        {
            // console.log("update_uiblock_list() glUIBlocks.length = 0");

            //
            // put default blocks
            //
            var uiBlocks = gl_SubgroupUIBlocks[0];
            if ( uiBlocks != undefined )
            {
                // console.log("update_uiblock_list() uiBlock[0]:"+uiBlocks);

                glUIBlocks = uiBlocks;
            }
        }
    }
    else
    {
        save_uisetting_button.setAttribute("style", "display:none;");
        restore_uisetting_button.setAttribute("style", "display:none;");
        copy_from_uisetting_dropdown.setAttribute("style", "display:none;");
        copy_to_uisetting_dropdown.setAttribute("style", "display:none;");
    }

    create_blocks();
}

function check_existed_block(uiblock)
{
    var found = false;

    if (glUIBlocks.length == 0)
    {
        return false;
    }
    else
    {
        // 檢查重複
        for(var irow=0; irow<glUIBlocks.length;irow++)
        {
            var uirowdata = glUIBlocks[irow];
            for(var icol=0; icol<uirowdata.length;icol++)
            {
                var existed_uiblock = uirowdata[icol];

                if ( existed_uiblock.id === uiblock.id )
                {
                    found = true;
                    break;
                }
            }
            if (found == true)
            {
                break;
            }
        }
    }

    return found;
}

function put_to_existed_block(irow, icol, uiblock)
{
    var uirowdata = glUIBlocks[irow];
    if ( uirowdata == undefined )
    {
        glUIBlocks[irow] = [];
        glUIBlocks[irow][icol] = uiblock;
    }
    else
    {
        var existed_uiblock = uirowdata[icol];
        if ( uirowdata == undefined )
        {
            glUIBlocks[irow][icol] = uiblock;
        }
        else
        {
            //
            // find empty space
            //
            var first_col = icol;
            for(var itestrow=irow; itestrow<10;itestrow++)
            {
                var uirowdata = glUIBlocks[itestrow];
                if ( uirowdata == undefined )
                {
                    // console.log("put_to_existed_block() found empty space, itestrow:"+itestrow+" itestcol:0");
                    glUIBlocks[itestrow] = [];
                    glUIBlocks[itestrow][0] = uiblock;
                    break;
                }

                var found = false;
                for(var itestcol=first_col; itestcol<4;itestcol++)
                {
                    var existed_uiblock = uirowdata[itestcol];
                    if ( existed_uiblock == undefined )
                    {
                        // console.log("put_to_existed_block() found empty space, itestrow:"+itestrow+" itestcol:"+itestcol);
                        glUIBlocks[itestrow][itestcol] = [];
                        glUIBlocks[itestrow][itestcol] = uiblock;
                        found = true;
                        break;
                    }
                }
                first_col = 0;
                if (found == true)
                {
                    break;
                }
            }
        }
    }
}

function load_subgroup_ui_info()
{
    gl_SubgroupUIBlocks = [];

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsubgroupuiblock=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(subgroupid, uiblock)
        {
            //console.log("index:"+index);
            //gl_SubgroupUIBlocks.push(uiblock);
            gl_SubgroupUIBlocks[subgroupid] = uiblock.uidata;
        });

        //create_blocks();

    });
    return false;
}

function create_blocks()
{
    var main_content = document.getElementById('main_content');
    while (main_content.firstChild)
    {
        main_content.removeChild(main_content.firstChild);
    }

    for(var index=0; index<glUIBlocks.length;index++)
    {
        var uirowdata = glUIBlocks[index];
        var rowuiid = "row_block_"+(index+1);

        //console.log("create_blocks() rowuiid:"+rowuiid);

        var UI_row = create_Sortable_row(rowuiid);
        main_content.appendChild(UI_row);

        for(var icol=0; icol<uirowdata.length;icol++)
        {
            var uiblock = uirowdata[icol];

            var UI_block = create_block(uiblock);
            UI_row.appendChild(UI_block);
        }

        $('#row_block_'+(index+1)).sortable({
        placeholder         : 'sort-highlight',
        connectWith         : '.connectedSortable',
        // handle              : '.box-header, .nav-tabs',
        forcePlaceholderSize: false,
        start               : uiblock_start,
        receive             : uiblock_receive,
        update              : uiblock_update,
        stop                : uiblock_stop,
        zIndex              : 999999
      }).disableSelection();

      init();
    }
}

// function touchHandler(event)
// {
//     var touch = event.changedTouches[0];
//
//     var simulatedEvent = document.createEvent("MouseEvent");
//         simulatedEvent.initMouseEvent({
//         touchstart: "mousedown",
//         touchmove: "mousemove",
//         touchend: "mouseup"
//     }[event.type], true, true, window, 1,
//         touch.screenX, touch.screenY,
//         touch.clientX, touch.clientY, false,
//         false, false, false, 0, null);
//
//     touch.target.dispatchEvent(simulatedEvent);
//     event.preventDefault();
// }

function touchHandler(event)
{
 var touches = event.changedTouches,
    first = touches[0],
    type = "";

     switch(event.type)
    {
        case "touchstart": type = "mousedown"; break;
        case "touchmove":  type="mousemove"; break;
        case "touchend":   type="mouseup"; break;
        default: return;
    }
    var simulatedEvent = document.createEvent("MouseEvent");
    simulatedEvent.initMouseEvent(type, true, true, window, 1,
                              first.screenX, first.screenY,
                              first.clientX, first.clientY, false,
                              false, false, false, 0/*left*/, null);

    first.target.dispatchEvent(simulatedEvent);
    event.preventDefault();
}

var clickms = 100;
var lastTouchDown = -1;

function init()
{
    document.addEventListener("touchstart", touchHandler, true);
    document.addEventListener("touchmove", touchHandler, true);
    document.addEventListener("touchend", touchHandler, true);
    document.addEventListener("touchcancel", touchHandler, true);
}


function create_Sortable_row(uiid)
{
    var UI_row = document.createElement("div");
    UI_row.setAttribute("class","row connectedSortable");
    UI_row.setAttribute("id",uiid);
    return UI_row;
}

function create_block(uiblock)
{
    var UI_block = document.createElement("div");
    UI_block.setAttribute("class","col-lg-2 col-md-4 col-sm-6 col-xs-6");
    UI_block.setAttribute("id",uiblock.id);

    var box = document.createElement("div");
    box.setAttribute("class","box box-solid "+uiblock.bgcolor);
    UI_block.appendChild(box);

    var box_header = document.createElement("div");
    box_header.setAttribute("class","box-header");
    box.appendChild(box_header);

    //============================
    // var header_title = document.createElement("label");
    // var textWidth = uiblock.title.length;
    // if (textWidth > 4)
    //     header_title.setAttribute("style","font-size: 31px;");
    // else
    //     header_title.setAttribute("style","font-size: 32px;");
    // header_title.textContent = uiblock.title;
    // // console.log("create_block()  uiblock.title:"+ uiblock.title+"  textWidth:"+textWidth);
    // box_header.appendChild(header_title);
    //============================
    var header_title = document.createElement("label");
    header_title.setAttribute("class","overflow-ellipsis");
    header_title.setAttribute("style","font-size:2vw;width: 180px;white-space: nowrap;overflow: hidden;");
    header_title.textContent = uiblock.title;
    box_header.appendChild(header_title);
    //============================


    var header_boxtool = document.createElement("label");
    header_boxtool.setAttribute("class","box-tools pull-right");
    box_header.appendChild(header_boxtool);

    var header_button = document.createElement("button");
    header_button.setAttribute("type","button");
    header_button.setAttribute("class","btn btn-sm "+uiblock.btncolor);
    header_button.setAttribute("data-widget","remove");
    header_button.setAttribute("onclick","box_close(\""+uiblock.id+"\");");
    header_boxtool.appendChild(header_button);

    var header_button_i = document.createElement("i");
    header_button_i.setAttribute("class","fa fa-times");
    header_button.appendChild(header_button_i);


    var box_body = document.createElement("div");
    box_body.setAttribute("class","box-body border-radius-none");
    box.appendChild(box_body);

    var body_div = document.createElement("div");
    body_div.setAttribute("class","inner");
    box_body.appendChild(body_div);

    var body_p = document.createElement("div");
    body_p.setAttribute("class","inner");
    body_p.setAttribute("style","width: 180px;white-space: nowrap;overflow: hidden;");
    body_p.textContent = uiblock.context;
    body_div.appendChild(body_p);


    var box_footer = document.createElement("div");
    box_footer.setAttribute("class","small-box-footer");
    box_footer.setAttribute("style","background-color: #f3f3f3e3;");
    box.appendChild(box_footer);

    //var footer_href = document.createElement("a");
    //footer_href.setAttribute("href",uiblock.action);
    //box_footer.appendChild(footer_href);

    var footer_row = document.createElement("div");
    footer_row.setAttribute("class","row");
    box_footer.appendChild(footer_row);

    var footer_col = document.createElement("div");
    footer_col.setAttribute("class","col-xs-12 text-center");
    footer_row.appendChild(footer_col);

    var footer_label = document.createElement("label");
    footer_label.setAttribute("style","font-size: 14px");
    footer_label.textContent = "點擊查看";
    footer_col.appendChild(footer_label);

    var footer_i = document.createElement("i");
    footer_i.setAttribute("class","fa fa-arrow-circle-right");
    footer_col.appendChild(footer_i);

    return UI_block;
}


function uiblock_start(e, ui)
{
    // console.log("uiblock_start() index:" + ui.item.index());
    // // Start of Sort Order
    // items.pre = $('.connectedSortable').sortable('toArray');
    //
    // log(e.type);
}

function uiblock_update(e, ui)
{
    // console.log("uiblock_update() index:" + ui.item.index());
    // // Start of Sort Order
    // items.post = $('.connectedSortable').sortable('toArray');
    //
    // log(e.type);
}

function uiblock_receive(e, ui)
{
    // console.log("uiblock_receive() index:" + ui.item.index());
    // Start of Sort Order
    //items.post = $('.connectedSortable').sortable('toArray');

    //log(e.type);
}

function uiblock_stop(e, ui)
{
    var newUIBlocks = [];

    var row_num = glUIBlocks.length;

    for(var rowindex=0;rowindex<row_num;rowindex++)
    {
        var rowuiid = "row_block_"+(rowindex+1);
        var ui_array = $('#'+rowuiid).sortable('toArray');

        var uirow = [];

        for(var i=0; i<ui_array.length;i++)
        {
            var uiid = ui_array[i];
            var uidata = getUIdata(uiid);
            uirow.push(uidata);
        }

        //console.log("uiblock_stop() rowindex:"+rowindex+" uirow.length:" + uirow.length);

        newUIBlocks.push(uirow);
    }

    //console.log("uiblock_stop() newUIBlocks.length:" + newUIBlocks.length);

    glUIBlocks = null;
    glUIBlocks = newUIBlocks;

    create_blocks();

    // update_uiblock_record();

}

function getUIdata(uiid)
{
    var uidata = null;
    var row_num = glUIBlocks.length;

    //console.log("getUIdata() uiid:" + uiid);

    for(var rowindex=0;rowindex<row_num;rowindex++)
    {
        var row = glUIBlocks[rowindex];

        for(var colindex=0; colindex<row.length;colindex++)
        {
            var uidata = row[colindex];
            if ( uidata.id === uiid )
            {
                //console.log("getUIdata() Found!");

                return uidata;
            }
        }
    }
    return null;
}

function box_close(uiblockid)
{
	//console.log("box_close() uiblockid:"+uiblockid);

    var newUIBlocks = [];

    var row_num = glUIBlocks.length;

    for(var rowindex=0;rowindex<row_num;rowindex++)
    {
        var row = glUIBlocks[rowindex];

        var uirow = [];

        for(var colindex=0; colindex<row.length;colindex++)
        {
            var uidata = row[colindex];
            if ( uidata.id != uiblockid )
            {
                uirow.push(uidata);
            }
        }
        newUIBlocks.push(uirow);
    }

    glUIBlocks = null;
    glUIBlocks = newUIBlocks;

    create_blocks();
}

function update_subgroupuiblock_record(str_subgroupids, jsonstring, reload)
{

    var SendDate = "";
    SendDate += "updatesubgroupuiblock=1";
    SendDate += '&project_id='+project_id;
    SendDate += '&subgroupids='+str_subgroupids;
    SendDate += '&uidata='+jsonstring;

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            alert("更新成功!");
            if (reload)
            {
                create_blocks();
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("無法更新!");
        },
      complete:
        function()
        {
        }
    });

    return false;
}

function save_uisetting()
{
    var jsonstring = JSON.stringify(glUIBlocks);

    var subgroupids = [];

    var count = 0;
    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroup = gl_usersubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);

        if ( id_row_checkbox.checked )
        {
            subgroupids[count] = subgroup.subgroupid;
            count++;
        }
    }
    var str_subgroupids = subgroupids.toString();

    update_subgroupuiblock_record(str_subgroupids, jsonstring, false);

    load_subgroup_ui_info();
}

function restore_uisetting()
{
    //
    // put default blocks
    //
    var uiBlocks = gl_SubgroupUIBlocks[0];
    if ( uiBlocks != undefined )
    {
        // console.log("update_uiblock_list() uiBlock[0]:"+uiBlocks);

        glUIBlocks = uiBlocks;
    }

    create_blocks();
}

// Make the dashboard widgets sortable Using jquery UI
// $('.connectedSortable').sortable({
  $('#row_1, #row_2').sortable({
  placeholder         : 'sort-highlight',
  connectWith         : '.connectedSortable',
  // handle              : '.box-header, .nav-tabs',
  forcePlaceholderSize: false,
  start               : uiblock_start,
  receive             : uiblock_receive,
  update              : uiblock_update,
  stop                : uiblock_stop,
  zIndex              : 999999
}).disableSelection();

$('.connectedSortable .box-header, .connectedSortable .nav-tabs-custom').css('cursor', 'move');


function open_disetting_page()
{
    var win = window.open('/index.php/vilsdisetting/'+project_id, '_blank');
    win.focus();
}
