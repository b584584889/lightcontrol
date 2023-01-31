/*
 * Author: ikki Chung
 * Date: 11 Jun 2020
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


console.log("project_id="+project_id);
console.log("gl_accountid="+gl_accountid);
console.log("gl_accountname="+gl_accountname);
console.log("gl_groupid="+gl_groupid);
console.log("gl_userid="+gl_userid);

var gl_EditMode = 0;

var glUIBlocks = [];

// var items = { "act":{1:[], 2:[]}, "pre":{1:[], 2:[]}, "post":{1:[], 2:[]} };

load_ui_info();

function load_ui_info()
{
    glUIBlocks = [];

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loaduiblock=1'+'&project_id='+project_id, function(uiblock)
    {
        $.each(uiblock.uidata, function(index, uirowdata)
        {
            glUIBlocks.push(uirowdata);
        });

        create_blocks();
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

        if (gl_EditMode == 1)
        {
            $('#row_block_'+(index+1)).sortable({
            // placeholder         : 'sort-highlight',
            connectWith         : '.connectedSortable',
            // handle              : '.box-header, .nav-tabs',
            delay: 1500,
            forcePlaceholderSize: true,
            start               : uiblock_start,
            receive             : uiblock_receive,
            update              : uiblock_update,
            stop                : uiblock_stop,
            zIndex              : 999999
          }).disableSelection();

          init();
        }
    }
}

function touchHandler(event)
{
 var touches = event.changedTouches,
    first = touches[0],
    type = "";

     switch(event.type)
    {
        case "touchstart":
            type = "mousedown";
            break;
        case "touchmove":
            type = "mousemove";
            break;
        case "touchend":
            type = "mouseup";
            break;
        default: return;
    }
    var simulatedEvent = document.createEvent("MouseEvent");
    simulatedEvent.initMouseEvent(type, true, true, window, 1,
                              first.screenX, first.screenY,
                              first.clientX, first.clientY, false,
                              false, false, false, 0/*left*/, null);

    first.target.dispatchEvent(simulatedEvent);
    // event.preventDefault();
}

function preventDefault(e)
{
    e.preventDefault();
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

    var header_title = document.createElement("label");
    var textWidth = uiblock.title.length;
    if (textWidth > 4)
        header_title.setAttribute("style","font-size: 31px;");
    else
        header_title.setAttribute("style","font-size: 32px;");

    header_title.textContent = uiblock.title;
    // console.log("create_block()  uiblock.title:"+ uiblock.title+"  textWidth:"+textWidth);

    box_header.appendChild(header_title);

    var header_boxtool = document.createElement("label");
    header_boxtool.setAttribute("class","box-tools pull-right");
    box_header.appendChild(header_boxtool);

    if (gl_EditMode == 1)
    {
        var header_button = document.createElement("button");
        header_button.setAttribute("type","button");
        header_button.setAttribute("class","btn btn-sm "+uiblock.btncolor);
        header_button.setAttribute("data-widget","remove");
        header_button.setAttribute("onclick","box_close(\""+uiblock.id+"\");");
        header_boxtool.appendChild(header_button);

        var header_button_i = document.createElement("i");
        header_button_i.setAttribute("class","fa fa-times");
        header_button.appendChild(header_button_i);
    }

    var box_body = document.createElement("div");
    box_body.setAttribute("class","box-body border-radius-none");
    box.appendChild(box_body);

    var body_div = document.createElement("div");
    body_div.setAttribute("class","inner");
    box_body.appendChild(body_div);

    var body_p = document.createElement("div");
    body_p.setAttribute("class","inner");
    body_p.textContent = uiblock.context;
    body_div.appendChild(body_p);


    var box_footer = document.createElement("div");
    box_footer.setAttribute("class","small-box-footer");
    box_footer.setAttribute("style","background-color: #f3f3f3e3;");
    box.appendChild(box_footer);

    if (gl_EditMode == 0)
    {
    var footer_href = document.createElement("a");
    footer_href.setAttribute("href",uiblock.action);
    box_footer.appendChild(footer_href);

    var footer_row = document.createElement("div");
    footer_row.setAttribute("class","row");
    footer_href.appendChild(footer_row);

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
    }

    return UI_block;
}

var gl_scrollPosition = [
  self.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft,
  self.pageYOffset || document.documentElement.scrollTop  || document.body.scrollTop
];

function noScroll()
{
    // console.log("noScroll() scrollPosition 0:"+scrollPosition[0]+" 1:"+scrollPosition[1]);
    // console.log("noScroll() scrollTop 0:"+document.documentElement.scrollTop[0]+" 1:"+document.documentElement.scrollTop[1]);
    console.log("noScroll() 0:"+gl_scrollPosition[0]+" 1:"+gl_scrollPosition[1]);

    window.scrollTo(gl_scrollPosition[0], gl_scrollPosition[1])
    // window.scrollTo(0, 0);
}

function uiblock_start(e, ui)
{
    // // horizontal scrolling amount
    // gl_scrollPosition[0] = window.pageXOffset;
    //
    // // vertical scrolling amount
    // gl_scrollPosition[1] = window.pageYOffset;

    // gl_scrollPosition = $(window).scrollTop();

    gl_scrollPosition = [
      self.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft,
      self.pageYOffset || document.documentElement.scrollTop  || document.body.scrollTop
    ];

    window.addEventListener('scroll', noScroll)

    // var vi_page_body = document.getElementById('vi_page_body');
    // vi_page_body.style.position = "fixed";

    // console.log("uiblock_start() index:" + ui.item.index());
    // // Start of Sort Order
    // items.pre = $('.connectedSortable').sortable('toArray');
    //
    // log(e.type);
}

function uiblock_update(e, ui)
{
    //console.log("uiblock_update() index:" + ui.item.index());
    // // Start of Sort Order
    // items.post = $('.connectedSortable').sortable('toArray');
    //
    // log(e.type);
}

function uiblock_receive(e, ui)
{
    //console.log("uiblock_receive() index:" + ui.item.index());
    // Start of Sort Order
    //items.post = $('.connectedSortable').sortable('toArray');

    //log(e.type);
}

function uiblock_stop(e, ui)
{

    window.removeEventListener('scroll', noScroll)

    // var vi_page_body = document.getElementById('vi_page_body');
    // vi_page_body.style.position = "";

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

    // console.log("uiblock_stop() newUIBlocks.length:" + newUIBlocks.length);

    glUIBlocks = null;
    glUIBlocks = newUIBlocks;

    create_blocks();

    update_uiblock_record();

    // console.log("uiblock_stop() index:" + ui.item.index());
    //items.post = $('.connectedSortable').sortable('toArray');
    //items.post[1] = $('#row_1').sortable('toArray');
    //items.post[2] = $('#row_2').sortable('toArray');
    //log(e.type);
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

    update_uiblock_record();

}

function update_uiblock_record()
{
    var jsonstring = JSON.stringify(glUIBlocks);

    var SendDate = "";
    SendDate += "updateuiblock=1";
    SendDate += '&project_id='+project_id;
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
        },
      error:
        function(xhr, status, error)
        {
        },
      complete:
        function()
        {
        }
    });

    return false;
}

function editblockbutton_click()
{
    console.log("editblockbutton_click()");

    var editblockbutton_ui = document.getElementById('editblockbutton');
    editblockbutton_ui.setAttribute("onclick", "exitblockbutton_click();return false;");

    while (editblockbutton_ui.firstChild)
    {
        editblockbutton_ui.removeChild(editblockbutton_ui.firstChild);
    }

    var editblockbutton_eiticon = document.createElement("i");
    editblockbutton_eiticon.setAttribute("class", "fa fa-edit");
    editblockbutton_ui.appendChild(editblockbutton_eiticon);
    editblockbutton_ui.appendChild(document.createTextNode("離開編輯模式"));

    var resettodefaultbutton_ui = document.getElementById('resettodefaultbutton');
    resettodefaultbutton_ui.setAttribute("style", "display:block;display:table;margin:0 auto;");

    gl_EditMode = 1;
    create_blocks();
}

function exitblockbutton_click()
{
    console.log("exitblockbutton_click()");

    var editblockbutton_ui = document.getElementById('editblockbutton');
    editblockbutton_ui.setAttribute("onclick", "editblockbutton_click();return false;");

    while (editblockbutton_ui.firstChild)
    {
        editblockbutton_ui.removeChild(editblockbutton_ui.firstChild);
    }

    var editblockbutton_eiticon = document.createElement("i");
    editblockbutton_eiticon.setAttribute("class", "fa fa-edit");
    editblockbutton_ui.appendChild(editblockbutton_eiticon);
    editblockbutton_ui.appendChild(document.createTextNode("編輯顯示框"));

    var resettodefaultbutton_ui = document.getElementById('resettodefaultbutton');
    resettodefaultbutton_ui.setAttribute("style", "display:none;margin:0 auto;");

    gl_EditMode = 0;
    create_blocks();
}

// $("#editblockbutton").click(function()
// {
//
// });

$("#resettodefaultbutton").click(function()
{
    var SendDate = "";
    SendDate += "resetuiblock=1";
    SendDate += '&project_id='+project_id;

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            var uiblock = JSON.parse(response);

            glUIBlocks = [];

            $.each(uiblock.uidata, function(index, uirowdata)
            {
                glUIBlocks.push(uirowdata);
            });

            create_blocks();
        },
      error:
        function(xhr, status, error)
        {
        },
      complete:
        function()
        {
        }
    });

    return false;
});

function showqrcode()
{
    device_qrcode_show("2", gl_accountname, gl_accountid, gl_userid);

    $('#modal-qrcode').modal('show');
}


function device_qrcode_show(type, accountname, accountid, userid)
{
    console.log('device_qrcode_show() type:'+type+" accountname:"+accountname+" accountid:"+accountid+" userid:"+userid);

    var getType = "TAG";
    if (type === '1')
        getType = "TAG";
    if (type === '2')
        getType = "USER";
    if (type === '3')
        getType = "LOCATOR";

    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: ' + getType;
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '名稱: ' + accountname;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '帳號: ' + accountid;

    var SendDate = "loadqrcode=1&type="+getType+"&accountid=" + accountid+"&id=" + userid+'&project_id='+project_id;
    var targetURL = gl_target_server + "/php/configvils.php";

    //console.log("get_qrcode() "+targetURL);

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            //console.log("sync_ERP_account() response:"+response);

            var result = JSON.parse(response);
            //console.log("sync_ERP_account() success:"+result.success+" qrcode:"+result.qrcode);

            var qrcode = document.getElementById("device_qrcode_ui");
            qrcode.setAttribute("src", result.qrcode);

            var device_qrcode_target = document.getElementById("device_qrcode_target");
            device_qrcode_target.textContent = 'target: ' + result.target;

            var device_qrcode_topic = document.getElementById("device_qrcode_topic");
            device_qrcode_topic.textContent = 'topic: ' + result.topic;

            var device_qrcode_datetime = document.getElementById("device_qrcode_datetime");
            device_qrcode_datetime.textContent = '更新時間: ' + result.datetime;

        }
    });
}

function confirm_reset_user_password()
{
    if (confirm('是否確定重新設定此帳號密碼:'+gl_accountname+' ?'))
    {
        var newValue= prompt("請輸入新密碼:[空白代表系統將自動產生]");
        // reset it!
        reset_user_password(gl_accountid, newValue);
    }
    else
    {
        // Do nothing!
    }
}


function reset_user_password(accountid, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['reset_user_password'] = 1;
    jsondata['userid'] = gl_userid;
    jsondata['account'] = accountid;
    jsondata['password'] = newValue;

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
                alert("更新密碼失敗!!!");
                console.log("update Error! response = " + JSON.stringify(response));
            }
            else
            {
                var result = JSON.parse(response);

                if ( result.account != undefined && result.password != undefined)
                {
                    alert("更新成功 新密碼為:"+ result.password);
                }
                else
                {
                    alert("更新密碼失敗!!!");
                }
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("更新密碼失敗!!!");
            console.log("error = " + error);
            console.log("xhr.responseText = " + xhr.responseText);
        }
    });
}

// function log(et) {
//     if (!et)
//     {
//       console.log("Activation - order: ", items.act.toString());
//     }
//     if (et == "sortstart")
//     {
//       console.log("Pre-Sort - order: ", items.pre.toString());
//     }
//     if (et == "sortupdate")
//     {
//       console.log("Post-Sort - order: ", items.post.toString());
//     }
//     if (et == "sortstop")
//     {
//       console.log("stop-Sort - order: ", items.post[1].toString());
//       console.log("stop-Sort - order: ", items.post[2].toString());
//     }
//   }

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

// Activation Order
// Activation Order
  // items.act[1] = $('#row_1').sortable('toArray');
  // items.act[2] = $('#row_2').sortable('toArray');
