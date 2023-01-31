/*
 * Author: ikki Chung
 * Date: 2020.07.09
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var unknownrfidcards_PreLoadIndex = -1;
var unknownrfidcards_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var unknownrfidcards_glCurrentPage = 1;

var gl_notOnLinelist_unknownrfidcards = [];
var gl_OnLinelist_unknownrfidcards = [];

check_unknownrfidcards_loading_status();

function check_unknownrfidcards_loading_status()
{
    //console.log("check_unknownrfidcards_loading_status() unknownrfidcards_PreLoadIndex:" + unknownrfidcards_PreLoadIndex +" unknownrfidcards_CurrentLoadIndex:" + unknownrfidcards_CurrentLoadIndex);
    switch(unknownrfidcards_CurrentLoadIndex)
    {
        case 0:
            if ( unknownrfidcards_PreLoadIndex != unknownrfidcards_CurrentLoadIndex )
            {
                // console.log("check_unknownrfidcards_loading_status() unknownrfidcards_PreLoadIndex:" + unknownrfidcards_PreLoadIndex +" != unknownrfidcards_CurrentLoadIndex:" + unknownrfidcards_CurrentLoadIndex);
                unknownrfidcards_PreLoadIndex = unknownrfidcards_CurrentLoadIndex;

                load_unknownrfidcard();
            }
        break;
        case 1:
            if ( unknownrfidcards_PreLoadIndex != unknownrfidcards_CurrentLoadIndex )
            {
                // console.log("check_unknownrfidcards_loading_status() unknownrfidcards_PreLoadIndex:" + unknownrfidcards_PreLoadIndex +" != unknownrfidcards_CurrentLoadIndex:" + unknownrfidcards_CurrentLoadIndex);
                unknownrfidcards_PreLoadIndex = unknownrfidcards_CurrentLoadIndex;

                draw_unknownrfidcard_table();

                unknownrfidcards_CurrentLoadIndex = unknownrfidcards_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( unknownrfidcards_PreLoadIndex != unknownrfidcards_CurrentLoadIndex )
            {
                // console.log("check_unknownrfidcards_loading_status() unknownrfidcards_PreLoadIndex:" + unknownrfidcards_PreLoadIndex +" != unknownrfidcards_CurrentLoadIndex:" + unknownrfidcards_CurrentLoadIndex);
                unknownrfidcards_PreLoadIndex = unknownrfidcards_CurrentLoadIndex;

                unknownrfidcards_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( unknownrfidcards_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_unknownrfidcards_loading_status, 100);
    else
    {
        // console.log("check_unknownrfidcards_loading_status() Stop.");

        setTimeout(reload_unknownrfidcard_info, 10000);
    }
}

function load_unknownrfidcard()
{
    if ( gl_activetab === 'activetab_unknown_uhfcards' )
    {
    }
    else
    {
        setTimeout(load_unknownrfidcard, 1000);
        return;
    }

    var offset = (unknownrfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_unknownrfidcards = [];
    gl_OnLinelist_unknownrfidcards = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadunknowncard=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_unknownrfidcards.push(device);
            else
                gl_OnLinelist_unknownrfidcards.push(device);
        });
    })
    .success(function() {
        unknownrfidcards_CurrentLoadIndex = unknownrfidcards_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_unknownrfidcard_table()
{
    var record_table = document.getElementById('UNKNOWN_RFID_CARD_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_unknownrfidcards.length; ++i)
        {
            var card = gl_OnLinelist_unknownrfidcards[i];
            var cardIndex = (unknownrfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = unknownrfidcards_create_record_table_item(cardIndex+i, card);
            record_table.appendChild(row);
        }
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_unknownrfidcards.length; ++i)
        {
            var card = gl_notOnLinelist_unknownrfidcards[i];
            var cardIndex = (unknownrfidcards_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = unknownrfidcards_create_record_table_item(cardIndex+i, card);
            record_table.appendChild(row);
        }
    }
}

function unknownrfidcards_create_record_table_item(ui_index, card)
{
    if (card == null)
        return null;

    var row = document.createElement('tr');

    if ( card.alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

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

    // cardid
    row.appendChild(document.createElement('td'));
    var SpanID = document.createElement("span");
    SpanID.setAttribute("id","uiid_name_"+card.cardid);
    SpanID.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanID.textContent = card.cardid;
    row.cells[cellIndex].appendChild(SpanID);
    cellIndex++;

    // 感應裝置名稱
    row.appendChild(document.createElement('td'));
    var SpanDeviceName = document.createElement("span");
    SpanDeviceName.setAttribute("class","badge bg-aqua-active");
    SpanDeviceName.textContent = getDeviceName(card.deviceid);
    row.cells[cellIndex].appendChild(SpanDeviceName);
    cellIndex++;

    // 感應時間
    row.appendChild(document.createElement('td'));
    var SpanUpdatetime = document.createElement("span");
    SpanUpdatetime.setAttribute("class","badge bg-green-active");
    SpanUpdatetime.textContent = card.updatetime;
    row.cells[cellIndex].appendChild(SpanUpdatetime);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    if (edit_node_info)
    {
        var deleteBtn = document.createElement("button");
        //deleteBtn.setAttribute("style","display:none");
        deleteBtn.setAttribute("id","uiid_deletebtn_"+card.cardid);
        deleteBtn.setAttribute("title","Delete This Card");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_unknownrfidcard(\""+card.cardid+"\");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-user-times");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);

        var registerBtn = document.createElement("button");
        registerBtn.setAttribute("id","uiid_registerbtn_"+card.cardid);
        registerBtn.setAttribute("title","註冊卡片");
        registerBtn.setAttribute("href","javascript:;");
        registerBtn.setAttribute("onclick","confirm_register_unknownrfidcard(\""+card.cardid+"\");return false;");
        var imgRegister = document.createElement("i");
        imgRegister.setAttribute("class","fa fa-check-square-o");
        registerBtn.appendChild(imgRegister);
        row.cells[cellIndex].appendChild(registerBtn);

        cellIndex++;
    }

    return row;
}

function confirm_delete_unknownrfidcard(cardid)
{
    if (confirm('Are you sure you want to delete this card:'+cardid+'   ?'))
    {
        // delete it!
        delete_unknownrfidcard(cardid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_unknownrfidcard(cardid)
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
            //setTimeout(reload_unknownrfidcard_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_register_unknownrfidcard(cardid)
{
    if (confirm('註冊卡片, 設定這張卡片到已註冊區? '+cardid+'   ?'))
    {
        // register it!
        register_unknownrfidcard(cardid);
    }
    else
    {
        // Do nothing!
    }
}

function register_unknownrfidcard(cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['register_card'] = 1;
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
            //setTimeout(reload_unknownrfidcard_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

// $("#deleteunknownrfidcardsbutton").click(function()
// {
//     if (confirm('確定刪除所有未註冊UHF卡片?'))
//     {
//         // delete it!
//         delete_all_unknownrfidcard();
//     }
//     else
//     {
//         // Do nothing!
//     }
//
//     return false;
// });

function delete_all_unknownrfidcard()
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_all_unknowncard'] = 1;

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
            //setTimeout(reload_unknownrfidcard_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_unknown_uhfcards_xlsx_report()
{
    var unknown_uhfcards_xlsxreport_status = document.getElementById("unknown_uhfcards_xlsxreport_status");
    unknown_uhfcards_xlsxreport_status.textContent = "產生報表中...";

    var SendDate = "loadunknownuhfcardsxlsxreport=1&project_id="+project_id;
    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            var result = JSON.parse(response);
            console.log("create_user_xlsx_report() success:"+result.success+" xlsxreport:"+result.xlsxreport);

            var unknown_uhfcards_xlsxreport_status = document.getElementById("unknown_uhfcards_xlsxreport_status");
            while (unknown_uhfcards_xlsxreport_status.firstChild)
            {
                unknown_uhfcards_xlsxreport_status.removeChild(unknown_uhfcards_xlsxreport_status.firstChild);
            }

            if ( result.success === 'true')
            {
                var xlsx_report_dl_btn = document.createElement("button");
                xlsx_report_dl_btn.setAttribute("type", 'button');
                xlsx_report_dl_btn.setAttribute("class", 'btn btn-app');
                xlsx_report_dl_btn.setAttribute("style", "width:90px;height:100px");
                xlsx_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.xlsxreport +"');");
                unknown_uhfcards_xlsxreport_status.appendChild(xlsx_report_dl_btn);

                var imgPdf = document.createElement("img");
                imgPdf.setAttribute("src", "/images/xlsx-dl-icon-1.png");
                imgPdf.setAttribute("alt", "Download report");
                imgPdf.setAttribute("title", "Download report");
                imgPdf.setAttribute("height", "80");
                imgPdf.setAttribute("width", "80");
                xlsx_report_dl_btn.appendChild(imgPdf);
            }
            else
            {
                var unknown_uhfcards_xlsxreport_status = document.getElementById("unknown_uhfcards_xlsxreport_status");
                unknown_uhfcards_xlsxreport_status.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function open_new_page(openurl)
{
    var win = window.open(openurl, '_blank');
    win.focus();
}

function reload_unknownrfidcard_info()
{
    unknownrfidcards_PreLoadIndex = -1;
    unknownrfidcards_CurrentLoadIndex = 0;
    check_unknownrfidcards_loading_status();
}

function unknown_rfidcards_move_page_backward()
{
    var current_page_num_ui = document.getElementById('unknown_rfidcards_id_current_page_num');

    if ( unknownrfidcards_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        unknownrfidcards_glCurrentPage--;
        current_page_num_ui.textContent = unknownrfidcards_glCurrentPage;
    }
}

function unknown_rfidcards_move_page_forward()
{
    var current_page_num_ui = document.getElementById('unknown_rfidcards_id_current_page_num');

    unknownrfidcards_glCurrentPage++;
    current_page_num_ui.textContent = unknownrfidcards_glCurrentPage;

}
