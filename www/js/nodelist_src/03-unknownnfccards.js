/*
 * Author: ikki Chung
 * Date: 2020.07.09
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var unknownnfccards_PreLoadIndex = -1;
var unknownnfccards_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var unknownnfccards_glCurrentPage = 1;

var gl_notOnLinelist_unknownnfccards = [];
var gl_OnLinelist_unknownnfccards = [];

check_unknownnfccards_loading_status();

function check_unknownnfccards_loading_status()
{
    switch(unknownnfccards_CurrentLoadIndex)
    {
        case 0:
            if ( unknownnfccards_PreLoadIndex != unknownnfccards_CurrentLoadIndex )
            {
                // console.log("check_unknownnfccards_loading_status() unknownnfccards_PreLoadIndex:" + unknownnfccards_PreLoadIndex +" != unknownnfccards_CurrentLoadIndex:" + unknownnfccards_CurrentLoadIndex);
                unknownnfccards_PreLoadIndex = unknownnfccards_CurrentLoadIndex;

                load_unknownnfccard();
            }
        break;
        case 1:
            if ( unknownnfccards_PreLoadIndex != unknownnfccards_CurrentLoadIndex )
            {
                // console.log("check_unknownnfccards_loading_status() unknownnfccards_PreLoadIndex:" + unknownnfccards_PreLoadIndex +" != unknownnfccards_CurrentLoadIndex:" + unknownnfccards_CurrentLoadIndex);
                unknownnfccards_PreLoadIndex = unknownnfccards_CurrentLoadIndex;

                draw_unknownnfccard_table();

                unknownnfccards_CurrentLoadIndex = unknownnfccards_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( unknownnfccards_PreLoadIndex != unknownnfccards_CurrentLoadIndex )
            {
                // console.log("check_unknownnfccards_loading_status() unknownnfccards_PreLoadIndex:" + unknownnfccards_PreLoadIndex +" != unknownnfccards_CurrentLoadIndex:" + unknownnfccards_CurrentLoadIndex);
                unknownnfccards_PreLoadIndex = unknownnfccards_CurrentLoadIndex;

                unknownnfccards_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( unknownnfccards_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_unknownnfccards_loading_status, 100);
    else
    {
        // console.log("check_unknownnfccards_loading_status() Stop.");

        setTimeout(reload_unknownnfccard_info, 10000);
    }
}

function load_unknownnfccard()
{
    if ( gl_activetab === 'activetab_unknown_nfccards' )
    {
    }
    else
    {
        setTimeout(load_unknownnfccard, 1000);
        return;
    }

    var offset = (unknownnfccards_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_unknownnfccards = [];
    gl_OnLinelist_unknownnfccards = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadunknowncard=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_unknownnfccards.push(device);
            else
                gl_OnLinelist_unknownnfccards.push(device);
        });
    })
    .success(function() {
        unknownnfccards_CurrentLoadIndex = unknownnfccards_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_unknownnfccard_table()
{
    var record_table = document.getElementById('UNKNOWN_NFC_CARD_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
         for (var i = 0; i < gl_OnLinelist_unknownnfccards.length; ++i)
        {
            var card = gl_OnLinelist_unknownnfccards[i];

            var cardIndex = (unknownnfccards_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = unknownnfccards_create_record_table_item(cardIndex + i, card);
            record_table.appendChild(row);
        }
        uiindex = gl_OnLinelist_unknownnfccards.length;
    }
    if (gl_showfilter_showoffline)
    {
         for (var i = 0; i < gl_notOnLinelist_unknownnfccards.length; ++i)
        {
            var card = gl_notOnLinelist_unknownnfccards[i];

            var cardIndex = (unknownnfccards_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
            var row = unknownnfccards_create_record_table_item(cardIndex + i, card);
            record_table.appendChild(row);
        }
    }
}

function unknownnfccards_create_record_table_item(ui_index, card)
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
    SpanDeviceName.setAttribute("class","badge bg-yellow");
    SpanDeviceName.textContent = getNFCDeviceName(card.deviceid);
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
        // delete card button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("id","uiid_deletebtn_"+card.cardid);
        deleteBtn.setAttribute("title","Delete This Card");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_unknownnfccard(\""+card.cardid+"\");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-user-times");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);

        var registerBtn = document.createElement("button");
        registerBtn.setAttribute("id","uiid_registerbtn_"+card.cardid);
        registerBtn.setAttribute("title","註冊卡片");
        registerBtn.setAttribute("href","javascript:;");
        registerBtn.setAttribute("onclick","confirm_register_unknownnfccard(\""+card.cardid+"\");return false;");
        var imgRegister = document.createElement("i");
        imgRegister.setAttribute("class","fa fa-check-square-o");
        registerBtn.appendChild(imgRegister);
        row.cells[cellIndex].appendChild(registerBtn);

        cellIndex++;
    }


    return row;
}

function confirm_delete_unknownnfccard(cardid)
{
    if (confirm('Are you sure you want to delete this card:'+cardid+'   ?'))
    {
        // delete it!
        delete_unknownnfccard(cardid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_unknownnfccard(cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_card'] = 1;
    jsondata['cardid'] = cardid;

    var targetURL = gl_target_server + "/php/nfclocator.php";

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

function confirm_register_unknownnfccard(cardid)
{
    if (confirm('註冊卡片, 設定這張卡片到已註冊區? '+cardid+'   ?'))
    {
        // register it!
        register_unknownnfccard(cardid);
    }
    else
    {
        // Do nothing!
    }
}

function register_unknownnfccard(cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['register_card'] = 1;
    jsondata['cardid'] = cardid;

    var targetURL = gl_target_server + "/php/nfclocator.php";

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

// $("#deleteunknownnfccardsbutton").click(function()
// {
//     if (confirm('確定刪除所有未註冊NFC卡片?'))
//     {
//         // delete it!
//         delete_all_unknownnfccard();
//     }
//     else
//     {
//         // Do nothing!
//     }
//
//     return false;
// });

function delete_all_unknownnfccard()
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_all_unknowncard'] = 1;

    var targetURL = gl_target_server + "/php/nfclocator.php";

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

function create_unknown_nfccards_xlsx_report()
{
    var unknown_nfccards_xlsxreport_status = document.getElementById("unknown_nfccards_xlsxreport_status");
    unknown_nfccards_xlsxreport_status.textContent = "產生報表中...";

    var SendDate = "loadunknownnfccardsxlsxreport=1&project_id="+project_id;

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
            console.log("create_unknown_nfccards_xlsx_report() success:"+result.success+" xlsxreport:"+result.xlsxreport);

            var unknown_nfccards_xlsxreport_status = document.getElementById("unknown_nfccards_xlsxreport_status");
            while (unknown_nfccards_xlsxreport_status.firstChild)
            {
                unknown_nfccards_xlsxreport_status.removeChild(unknown_nfccards_xlsxreport_status.firstChild);
            }

            if ( result.success === 'true')
            {
                var xlsx_report_dl_btn = document.createElement("button");
                xlsx_report_dl_btn.setAttribute("type", 'button');
                xlsx_report_dl_btn.setAttribute("class", 'btn btn-app');
                xlsx_report_dl_btn.setAttribute("style", "width:90px;height:100px");
                xlsx_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.xlsxreport +"');");
                unknown_nfccards_xlsxreport_status.appendChild(xlsx_report_dl_btn);

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
                var unknown_nfccards_xlsxreport_status = document.getElementById("xlsxreport_status");
                unknown_nfccards_xlsxreport_status.textContent = "產生報表失敗!";
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

function reload_unknownnfccard_info()
{
    unknownnfccards_PreLoadIndex = -1;
    unknownnfccards_CurrentLoadIndex = 0;
    check_unknownnfccards_loading_status();
}

function unknown_nfccards_move_page_backward()
{
    var current_page_num_ui = document.getElementById('unknown_nfccards_id_current_page_num');

    if ( unknownnfccards_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        unknownnfccards_glCurrentPage--;
        current_page_num_ui.textContent = unknownnfccards_glCurrentPage;

        reload_unknownnfccard_info();
    }
}

function unknown_nfccards_move_page_forward()
{
    var current_page_num_ui = document.getElementById('unknown_nfccards_id_current_page_num');

    unknownnfccards_glCurrentPage++;
    current_page_num_ui.textContent = unknownnfccards_glCurrentPage;

    reload_unknownnfccard_info();

}
