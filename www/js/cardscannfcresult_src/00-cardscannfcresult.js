
/*
 * Author: ikki Chung
 * Date: 2020.07.13
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

//
// globa value
//
var glAllCardScanResult = {};

var gl_nfclocators = [];
var gl_nfccards = [];
var gl_user_info = [];
var gl_cardscanresult = [];

var LOAD_CARD_NUM = 300;
var gl_nfccard_load_index = 0;

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 999;
var MAX_SHOW_RESULT = 20;

var LOAD_PAGE_NUM = 20;
var gl_CurrentPage = 1;

loading_status();

function loading_status()
{
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                PreLoadIndex = CurrentLoadIndex;

                load_cardscan_result();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                PreLoadIndex = CurrentLoadIndex;

                show_cardscan_result();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                PreLoadIndex = CurrentLoadIndex;

                load_nfccard();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();


            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(loading_status, 100);
    else
    {
        //console.log("loading_status() Stop.");
        setTimeout(reload_cardscan_result, 5000);
    }
}

function reload_cardscan_result()
{
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
    loading_status();
}

function load_nfclocator()
{
    gl_nfclocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_nfccard()
{
    // var querycard = '';
    var queryCardList = [];
    // console.log("load_nfccard() gl_cardscanresult.length:"+gl_cardscanresult.length);

    // var queryCount = 0;
    for (var i = 0; i < gl_cardscanresult.length; ++i)
    {
        var card = gl_cardscanresult[i];


        // console.log("load_nfccard() gl_nfccards.length:"+gl_nfccards.length);

        //
        // check exist
        //
        var bFound = false;
        for (var j = 0; j < gl_nfccards.length; ++j)
        {
            var existed_card = gl_nfccards[j];
            if ( existed_card.cardid === card.cardid )
            {
                bFound = true;
                break;
            }
        }

        if ( bFound == false)
        {
            for (var j = 0; j < queryCardList.length; ++j)
            {
                var existed_card = queryCardList[j];
                if ( existed_card === card.cardid )
                {
                    bFound = true;
                    break;
                }
            }
        }

        if ( bFound == false)
        {
            queryCardList.push(card.cardid);
            //
            // if ( queryCount > 0 )
            //     querycard += ',';
            // querycard += card.cardid;
            // queryCount += 1;
        }
    }

    if (queryCardList.length == 0)
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    querycard = queryCardList.toString();

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'querycards=1&cardid='+querycard+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_nfccards.push(device);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function load_user_info()
{
    var queryuser = '';

    var queryCount = 0;
    for (var i = 0; i < gl_nfccards.length; ++i)
    {
        var card = gl_nfccards[i];
        var card_user_id = parseInt(card.userid);
        //
        // check exist
        //
        var bFound = false;
        for (var j = 0; j < gl_user_info.length; ++j)
        {
            var existed_user = gl_user_info[j];
            if ( existed_user.userid == card_user_id )
            {
                bFound = true;
                break;
            }
        }

        if ( bFound == false)
        {
            if (card_user_id != 0)
            {
                if ( queryCount > 0 )
                    queryuser += ',';
                queryuser += card.userid;
                queryCount += 1;
            }
        }
    }

    if (queryuser.length == 0)
    {
        CurrentLoadIndex = LOAD_STATE_END;
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'queryusers=1&userid='+queryuser+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);
        });
    })
    .success(function() {

        //reload_cardscan_result();

        PreLoadIndex = 0;
        CurrentLoadIndex = 1;

    })
    .error(function() {
    })
    .complete(function() {
    });
}
// function load_nfccard()
// {
//     var card_num = 0;
//
//     var targetURL = gl_target_server + "/php/nfclocator.php";
//     var p = $.getJSON(targetURL, 'loadcard=1&s='+gl_nfccard_load_index+'&count='+LOAD_CARD_NUM+'&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, device)
//         {
//             gl_nfccards.push(device);
//             card_num += 1;
//         });
//
//         gl_nfccard_load_index += card_num;
//     })
//     .success(function() {
//         if (card_num < LOAD_CARD_NUM)
//         {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         }
//         else
//         {
//             setTimeout(load_nfccard, 20);
//         }
//     })
//     .error(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//     })
//     .complete(function() {
//     });
// }

// function load_user_info()
// {
//     var user_num = 0;
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, user)
//         {
//             user.userid = parseInt(user.userid);
//             gl_user_info.push(user);
//
//             user_num += 1;
//         });
//
//         gl_user_load_index += user_num;
//     })
//     .success(function() {
//         if (user_num < LOAD_USER_NUM)
//         {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         }
//         else
//         {
//             setTimeout(load_user_info, 20);
//         }
//     })
//     .error(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//     })
//     .complete(function() {
//     });
// }

function load_cardscan_result()
{
    var offset = (gl_CurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_cardscanresult = [];

    var input_value = document.getElementById("search_nfccard_text").value;

    // console.log("load_cardscan_result() input_value:"+input_value);

    var queryString = 'loadcardscan=1&s='+offset+'&count='+count+'&project_id='+project_id;
    if (input_value.length > 0)
    {
        queryString = 'loadcardscan=1&s='+offset+'&count='+count+'&project_id='+project_id +'&matchname='+input_value;
    }

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, queryString, function(data)
    {
        var showMsg = "";
        $.each(data, function(index, value)
        {
            gl_cardscanresult.push(value);

            // console.log("load_cardscan_result() value.cardid:"+value.cardid+" LoRAMacaddress:"+value.LoRAMacaddress);

        });

        //console.log("load_cardscan_result() gl_cardscanresult.length:"+gl_cardscanresult.length);
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_cardscan_result()
{
    document.getElementById("CARDSCAN_TABLE_MSG").innerText = "";

    var cardscan_table = document.getElementById('CARDSCAN_TABLE');

    while (cardscan_table.rows.length >= 1)
         cardscan_table.deleteRow(0);

     for (var i = 0; i < gl_cardscanresult.length; ++i)
     {
         var card = gl_cardscanresult[i];

         var cardIndex = (gl_CurrentPage - 1) * LOAD_PAGE_NUM;
         var row = nfccards_create_record_table_item(cardIndex + i, card);
         cardscan_table.appendChild(row);
     }
}

function nfccards_create_record_table_item(ui_index, card)
{
    if (card == null)
        return null;

    var row = document.createElement('tr');

    // if ( card.alive === '0' )
    //     row.setAttribute("bgcolor","#a3c2c2");
    // else
    //     row.setAttribute("bgcolor","#FFFFFF");

    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    //var ShowName = getNFCCardName(card.cardid);
    var nfccard = getNFCCard(card.cardid);
    var ShowName = card.cardid;
    var UserName = "";
    if (nfccard != null)
    {
        ShowName = nfccard.name;
        if (nfccard.userid > 0)
        {
            ShowName = getUserName(nfccard.userid);
            UserName = ShowName;
        }
        if ( ShowName.length == 0 )
            ShowName = card.cardid;
    }
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // cardname
    row.appendChild(document.createElement('td'));
    var SpanName = document.createElement("span");
    SpanName.setAttribute("class","badge bg-blue-active");
    SpanName.textContent = ShowName;
    row.cells[cellIndex].appendChild(SpanName);
    cellIndex++;

    // id
    row.appendChild(document.createElement('td'));
    var SpanId = document.createElement("span");
    SpanId.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanId.textContent = card.cardid;
    row.cells[cellIndex].appendChild(SpanId);
    cellIndex++;

    // 感應裝置名稱
    row.appendChild(document.createElement('td'));
    var SpanDeviceName = document.createElement("span");
    SpanDeviceName.setAttribute("class","badge bg-blue");
    SpanDeviceName.textContent = getNFCDeviceName(card.LoRAMacaddress);
    row.cells[cellIndex].appendChild(SpanDeviceName);
    cellIndex++;

    // 人員
    row.appendChild(document.createElement('td'));
    var SpanUserName = document.createElement("span");
    if (UserName.length > 0)
    {
        SpanUserName.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
        SpanUserName.textContent = UserName;
    }
    row.cells[cellIndex].appendChild(SpanUserName);
    cellIndex++;

    // 感應時間
    row.appendChild(document.createElement('td'));
    var Spandatetime = document.createElement("span");
    Spandatetime.setAttribute("class","badge bg-green-active");
    Spandatetime.textContent = card.datetime;
    row.cells[cellIndex].appendChild(Spandatetime);
    cellIndex++;

    // SSID
    row.appendChild(document.createElement('td'));
    var SpanSSID = document.createElement("span");
    //SpanSSID.setAttribute("class","badge bg-green-active");
    SpanSSID.textContent = card.ssid;
    row.cells[cellIndex].appendChild(SpanSSID);
    cellIndex++;

    // AP mac
    row.appendChild(document.createElement('td'));
    var SpanWifimac = document.createElement("span");
    //SpanWifimac.setAttribute("class","badge bg-green-active");
    SpanWifimac.textContent = card.wifimac;
    row.cells[cellIndex].appendChild(SpanWifimac);
    cellIndex++;

    return row;
}

function getNFCCard(cardid)
{
    for (var i = 0; i < gl_nfccards.length; i++)
    {
        var card = gl_nfccards[i];
        if ( card.cardid === cardid )
        {
            return card;
        }
    }

    return null;
}

function getNFCCardName(cardid)
{
    for (var i = 0; i < gl_nfccards.length; i++)
    {
        var card = gl_nfccards[i];
        if ( card.cardid === cardid )
        {
            if (card.name.length == 0)
                return card.cardid;
            else
                return card.name;
        }
    }

    return cardid;
}

function getNFCDeviceName(LoRAMacaddress)
{
    for (var i = 0; i < gl_nfclocators.length; i++)
    {
        var nfclocator = gl_nfclocators[i];
        if ( nfclocator.LoRAMacaddress === LoRAMacaddress )
            return nfclocator.name;
    }

    return LoRAMacaddress;
}

function getUserName(userid)
{
    var UserName = "";

    for (var i = 0; i < gl_user_info.length; i++)
    {
        if ( userid == gl_user_info[i].userid)
        {
            //console.log('findUserName() gl_user_info[i].userid:'+gl_user_info[i].userid);

            UserName = gl_user_info[i].name;
            break;
        }
    }
    return UserName;
}

function move_page_backward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    if ( gl_CurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        gl_CurrentPage--;
        current_page_num_ui.textContent = gl_CurrentPage;

        reload_cardscan_result();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    gl_CurrentPage++;
    current_page_num_ui.textContent = gl_CurrentPage;

    reload_cardscan_result();

}

function search_nfccard_text_change(e)
{
    glCurrentPage = 1;

    reload_cardscan_result();

    return false;
}

const $source = document.querySelector('#search_nfccard_text');
$source.addEventListener('input', search_nfccard_text_change);// register for oninput

function open_cardscannfcresult_page()
{
    var win = window.open('/index.php/vilscardscannfcresult/'+project_id, '_blank');
    win.focus();
}
