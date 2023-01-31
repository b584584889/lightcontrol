/*
 * Author: ikki Chung
 * Date: 2020.05.04
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var rfidcards_PreLoadIndex = -1;
var rfidcards_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var rfidcards_glCurrentPage = 1;

var gl_notOnLinelist_rfidcards = [];
var gl_OnLinelist_rfidcards = [];

check_rfidcards_loading_status();

function check_rfidcards_loading_status()
{
    //console.log("check_rfidcards_loading_status() rfidcards_PreLoadIndex:" + rfidcards_PreLoadIndex +" rfidcards_CurrentLoadIndex:" + rfidcards_CurrentLoadIndex);
    switch(rfidcards_CurrentLoadIndex)
    {
        case 0:
            if ( rfidcards_PreLoadIndex != rfidcards_CurrentLoadIndex )
            {
                // console.log("check_rfidcards_loading_status() rfidcards_PreLoadIndex:" + rfidcards_PreLoadIndex +" != rfidcards_CurrentLoadIndex:" + rfidcards_CurrentLoadIndex);
                rfidcards_PreLoadIndex = rfidcards_CurrentLoadIndex;

                //load_rfidcard_info();
                load_rfidcard();
            }
        break;
        case 1:
            if ( rfidcards_PreLoadIndex != rfidcards_CurrentLoadIndex )
            {
                // console.log("check_rfidcards_loading_status() rfidcards_PreLoadIndex:" + rfidcards_PreLoadIndex +" != rfidcards_CurrentLoadIndex:" + rfidcards_CurrentLoadIndex);
                rfidcards_PreLoadIndex = rfidcards_CurrentLoadIndex;

                draw_rfidcard_table();

                rfidcards_CurrentLoadIndex = rfidcards_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( rfidcards_PreLoadIndex != rfidcards_CurrentLoadIndex )
            {
                // console.log("check_rfidcards_loading_status() rfidcards_PreLoadIndex:" + rfidcards_PreLoadIndex +" != rfidcards_CurrentLoadIndex:" + rfidcards_CurrentLoadIndex);
                rfidcards_PreLoadIndex = rfidcards_CurrentLoadIndex;

                rfidcards_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( rfidcards_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_rfidcards_loading_status, 100);
    else
    {
        // console.log("check_rfidcards_loading_status() Stop.");

        setTimeout(reload_rficcard_info, 10000);

        // if (gl_rfidcard_reload == 0)
        //     setTimeout(reload_rficcard_info, 3000);
        // else
        //     setTimeout(reload_rficcard_info, 30000);
        // gl_rfidcard_reload += 1;

    }
}

function load_rfidcard()
{

    if ( gl_activetab === 'activetab_uhfcards' )
    {
    }
    else
    {
        setTimeout(load_rfidcard, 1000);
        return;
    }

    var offset = (rfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_rfidcards = [];
    gl_OnLinelist_rfidcards = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcard=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_rfidcards.push(device);
            else
                gl_OnLinelist_rfidcards.push(device);
        });
    })
    .success(function() {
        rfidcards_CurrentLoadIndex = rfidcards_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

// function create_empty_record_table_item(ui_index)
// {
//     var empty_record_table_item = document.getElementById('empty_record_table_item');
//     if ( empty_record_table_item != null )
//         return;
//
//     var record_table = document.getElementById('RFID_CARD_TABLE');
//
//     var row = document.createElement('tr');
//     row.setAttribute("id", "empty_record_table_item");
//
//     var cellIndex = 0;
//
//     // index
//     row.appendChild(document.createElement('td'));
//     row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
//     cellIndex++;
//
//     // id
//     row.appendChild(document.createElement('td'));
//     row.cells[cellIndex].appendChild(create_input_text("input_id"));
//     cellIndex++;
//
//     // save button
//     row.appendChild(document.createElement('td'));
//     var Button_save = document.createElement("button");
//     Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
//     Button_save.setAttribute("onclick","save_new_card();");
//     Button_save.textContent = "儲存";
//     row.cells[cellIndex].appendChild(Button_save);
//     cellIndex++;
//
//     record_table.appendChild(row);
// }

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function draw_rfidcard_table()
{
    var record_table = document.getElementById('RFID_CARD_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_rfidcards.length; ++i)
        {
            var card = gl_OnLinelist_rfidcards[i];
            var cardIndex = (rfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = rfidcards_create_record_table_item(cardIndex+i, card);
            record_table.appendChild(row);
        }
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_rfidcards.length; ++i)
        {
            var card = gl_notOnLinelist_rfidcards[i];
            var cardIndex = (rfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = rfidcards_create_record_table_item(cardIndex+i, card);
            record_table.appendChild(row);
        }
    }
}

function rfidcards_create_record_table_item(ui_index, card)
{
    if (card == null)
        return null;

    var row = document.createElement('tr');

    if ( card.alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    // if ((ui_index % 2) == 1)
    //     row.setAttribute("bgcolor","#ebebe0");

    var ShowName = card.cardid;
    if ( card.userid === '0' )
    {

    }
    else
    {
        ShowName = findUserName(card.userid);
    }

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // alive
    row.appendChild(document.createElement('td'));
    if ( card.alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = card.alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // cardname
    row.appendChild(document.createElement('td'));
    var SpanName = document.createElement("span");
    SpanName.setAttribute("id","uiid_name_"+card.cardid);
    SpanName.setAttribute("class","badge bg-yellow-active");
    if (card.name.length > 0)
    {
        SpanName.textContent = card.name;
    }
    else
    {
        SpanName.textContent = ShowName;
    }
    row.cells[cellIndex].appendChild(SpanName);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id","uiid_name_"+card.cardid+"_NODNAME_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_rfidcard_name(\""+ card.cardid +"\",\""+ ShowName+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // cardytpe
    var tdid = document.createElement('td');
    tdid.setAttribute("id","uiid_type_"+card.cardid);
    row.appendChild(tdid);
    var SpanType = document.createElement("span");
    SpanType.setAttribute("class","badge bg-green-active");
    if ( card.userid === '0' )
    {
        SpanType.textContent = "卡片";
    }
    else
    {
        SpanType.textContent = "人員配戴";
    }
    tdid.appendChild(SpanType);
    cellIndex++;

    // id
    var tdid = document.createElement('td');
    tdid.setAttribute("id","uiid_id_"+card.cardid);
    row.appendChild(tdid);
    var SpanId = document.createElement("span");
    SpanId.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanId.textContent = card.cardid;
    tdid.appendChild(SpanId);
    cellIndex++;

    // userid
    var tdid = document.createElement('td');
    tdid.setAttribute("id","uiid_id_"+card.cardid);
    row.appendChild(tdid);

    if ( card.userid === '0' )
    {

    }
    else
    {
        var Spanuserid = document.createElement("span");
        Spanuserid.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
        Spanuserid.textContent = ShowName;
        tdid.appendChild(Spanuserid);
    }
    cellIndex++;

    // map layer
    row.appendChild(document.createElement('td'));
    var span_maplayer = document.createElement("span");
    span_maplayer.setAttribute("id","uiid_maplayer_"+card.cardid);
    span_maplayer.setAttribute("class","label label-primary");
    span_maplayer.textContent = get_maplayer_name(card.maplayer);
    row.cells[cellIndex].appendChild(span_maplayer);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    //
    // show QR Code
    //
    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_card_qrcode_show(\""+ card.cardid +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);

    // Edit button
    // var editBtn = document.createElement("button");
    // editBtn.setAttribute("id","uiid_editbtn_"+card.cardid);
    // editBtn.setAttribute("title","Edit this User");
    // editBtn.setAttribute("href","javascript:;");
    // editBtn.setAttribute("onclick","edit_card(\""+card.cardid+"\");return false;");
    // var imgEdit = document.createElement("i");
    // imgEdit.setAttribute("id","uiid_editimg_"+card.cardid);
    // imgEdit.setAttribute("class","fa fa-edit");
    // editBtn.appendChild(imgEdit);
    // row.cells[cellIndex].appendChild(editBtn);

    // Edit button
    if (edit_node_info)
    {
        var deleteBtn = document.createElement("button");
        //deleteBtn.setAttribute("style","display:none");
        deleteBtn.setAttribute("id","uiid_deletebtn_"+card.cardid);
        deleteBtn.setAttribute("title","Delete This Card");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_card(\""+card.cardid+"\");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-user-times");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);
        cellIndex++;
    }

    return row;
}

function edit_rfidcard_name(cardid, cardname)
{
    console.log("edit_rfidcard_name() cardid:"+cardid+" cardname:"+cardname);

    var newValue= prompt("編輯UHF卡片名稱:", cardname);
    if ( newValue )
    {
        var element_id = "uiid_name_"+cardid;

        document.getElementById(element_id).innerText = newValue;

        update_rfidcard_name(cardid, newValue);

    }
}

function device_card_qrcode_show(cardid)
{
    console.log('device_card_qrcode_show() cardid:'+cardid);

    var getType = "CARD";

    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: ' + getType;
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '';
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '卡片ID: ' + cardid;

    var SendDate = "loadqrcode=1&type="+getType+"&accountid=" + cardid+"&id=" + cardid+'&project_id='+project_id;
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

function update_rfidcard_name(cardid, newValue)
{
    if ( newValue.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_rfidcard_name'] = 1;
    jsondata['cardid'] = cardid;
    jsondata['cardname'] = newValue;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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
}

function confirm_delete_card(cardid)
{
    if (confirm('Are you sure you want to delete this card:'+cardid+'   ?'))
    {
        // delete it!
        delete_card(cardid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_card(cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_card'] = 1;
    jsondata['cardid'] = cardid;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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
            //setTimeout(reload_rficcard_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

$("#scannewrfidcardbutton").click(function()
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['scan_rfidcard'] = 1;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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
            //setTimeout(reload_rficcard_info, 3000);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

    return false;
});

function reload_rficcard_info()
{
    rfidcards_PreLoadIndex = -1;
    rfidcards_CurrentLoadIndex = 0;
    check_rfidcards_loading_status();
}

function rfidcards_move_page_backward()
{
    var current_page_num_ui = document.getElementById('rfidcards_id_current_page_num');

    if ( rfidcards_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        rfidcards_glCurrentPage--;
        current_page_num_ui.textContent = rfidcards_glCurrentPage;

        //reload_rficcard_info();
    }
}

function rfidcards_move_page_forward()
{
    var current_page_num_ui = document.getElementById('rfidcards_id_current_page_num');

    rfidcards_glCurrentPage++;
    current_page_num_ui.textContent = rfidcards_glCurrentPage;

    //reload_rficcard_info();

}

function open_rfidcards_page()
{
    var win = window.open('/index.php/vilsrfidcards/'+project_id, '_blank');
    win.focus();
}
