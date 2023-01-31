/*
 * Author: ikki Chung
 * Date: 12 Jun 2020
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var gl_taginfo = [];
var gl_userinfo = {};

load_tag_info();
load_user();

function load_tag_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);
        for (var i = 0; i<tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            var nodeData = node.split(",");

            if (node == undefined)
                continue;

            gl_taginfo.push(nodeData);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_user()
{
    gl_users = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&accountid='+accountid+'&project_id='+project_id, function(data)
    {
        gl_userinfo = data;

    })
    .success(function() {
        draw_cards();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_cards()
{
    var card_list = document.getElementById('card_list');

    var uwbcards = gl_userinfo.uwbcards.split(",");
    var rfidcards = gl_userinfo.rfidcards.split(",");
    var nfccards = gl_userinfo.nfccards.split(",");

    for (var i = 0; i < uwbcards.length; ++i)
    {
        var uwbcard = uwbcards[i];
        var cardname = getNodeName(uwbcard);
        var uwbcardui = create_ui("UWB", "bg-green", uwbcard, cardname);
        card_list.appendChild(uwbcardui);
    }

    for (var i = 0; i < rfidcards.length; ++i)
    {
        var rfidcard = rfidcards[i];
        var cardname = rfidcard;
        var rfidcardui = create_ui("RFID", "bg-yellow", rfidcard, cardname);
        card_list.appendChild(rfidcardui);
    }

    for (var i = 0; i < nfccards.length; ++i)
    {
        var nfccard = nfccards[i];
        var cardname = nfccard;
        var nfccardui = create_ui("NFC", "bg-aqua", nfccard, cardname);
        card_list.appendChild(nfccardui);
    }
}

function create_ui(showName, bgcolor, cardmac, cardname)
{
    var uwbcardui = document.createElement('div');
    uwbcardui.setAttribute("class","col-md-3 col-sm-6 col-xs-12");

    var box = document.createElement('div');
    box.setAttribute("class","info-box");
    uwbcardui.appendChild(box);

    var icon = document.createElement('span');
    icon.setAttribute("class","info-box-icon " + bgcolor);
    box.appendChild(icon);

    var iconText = document.createElement('SUP');
    iconText.setAttribute("style","font-size: 20px");
    iconText.textContent = showName;
    icon.appendChild(iconText);

    var content = document.createElement('div');
    content.setAttribute("class","info-box-content");
    box.appendChild(content);

    var macaddress = document.createElement('span');
    macaddress.setAttribute("class","info-box-text");
    macaddress.textContent = cardmac;
    content.appendChild(macaddress);

    var name = document.createElement('span');
    name.setAttribute("class","info-box-number");
    name.textContent = cardname;
    content.appendChild(name);

    return uwbcardui;
}

function getNodeName(uwbcard)
{
    for (var i = 0; i < gl_taginfo.length; i++)
    {
        var nodeData = gl_taginfo[i];
        var nodename = nodeData[5];
        var macaddress = nodeData[6];

        // console.log("getNodeName() uwbcard:"+uwbcard+" macaddress:"+macaddress);

        if ( macaddress === uwbcard )
        {
            return nodename;
        }
    }
    return uwbcard;
}
