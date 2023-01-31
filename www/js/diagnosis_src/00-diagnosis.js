/*
 * Author: ikki Chung
 * Date: 13 Aug 2018
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var gl_tag_num = 0;
var gl_locator_num = 0;
var gl_anchor_num = 0;

var gl_tag_load_index = 0;
var gl_locator_load_index = 0;
var gl_anchor_load_index = 0;

var glTag_List = [];
var glLocator_List = [];
var glAnchor_List = [];

var glAllAnchorsData = {};
var glAllAnchorsPreData = {};
var glAllAnchorsUpdateTime = {};

var glAllGatewayData = {};

var glAnchorUIList = [];
// var glGatewayUIList = [];

var glCurrentTagMacaddress = "";
var glCurrentLocatorLoRAMacaddress = "";

var LOAD_DEVICE_NUM = 20;

load_devices();

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        gl_tag_load_index = 0;
        gl_locator_load_index = 0;
        gl_anchor_load_index = 0;

        glTag_List = [];
        glLocator_List = [];
        glAnchor_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function load_tag()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        setTimeout(load_locator, 20);
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

        console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            glTag_List.push(nodeData);
        }

        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            setTimeout(load_locator, 20);
        }
        else
        {
            setTimeout(load_tag, 1);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tag, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}


function load_locator()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        setTimeout(load_anchor, 10);
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

        console.log("locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            glLocator_List.push(nodeData);
        }

        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {
            setTimeout(load_anchor, 10);
        }
        else
        {
            setTimeout(load_locator, 1);
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        setTimeout(load_locator, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}


function load_anchor()
{

    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(put_tags_to_dropdownlist, 10);
        setTimeout(put_locators_to_dropdownlist, 10);

        setTimeout(load_ranging_Diagnosis, 100);
        setTimeout(load_locator_rssiinfo, 1000);
        return;
    }
    if (gl_anchor_load_index == 0)
    {
        glAnchor_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        //console.log("load_anchors() anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            //console.log("load_anchors() key:"+key +", data[key]:"+data[key]);

            // split data
            var node = data[key];
            var nodeData = node.split(",");
            glLocator_List.push(nodeData);
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {

        //console.log("load_anchors() success gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        if (gl_anchor_load_index >= gl_anchor_num)
        {
            setTimeout(put_tags_to_dropdownlist, 10);
            setTimeout(put_locators_to_dropdownlist, 10);

            setTimeout(load_ranging_Diagnosis, 100);
            setTimeout(load_locator_rssiinfo, 1000);
        }
        else
        {
            setTimeout(load_anchor, 10);
        }
    })
    .error(function() {

        console.log("load_anchors() error gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        gl_anchor_load_index = 0;
        setTimeout(load_locator, 10);
    })
    .complete(function() {
    });
}

function put_tags_to_dropdownlist()
{

    var dropdown_tags_ul = document.getElementById("dropdown_tags");

    var index;
    for	(index = 0; index < glTag_List.length; index++)
    {
        nodeData = glTag_List[index];
        var macaddress = nodeData[6];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var defaultchecked = false;
        if (index == 0 )
        {
            glCurrentTagMacaddress = macaddress;
            defaultchecked = true;
            document.getElementById("dropdown_cur_tag").innerHTML = nodename + '<span class="caret"></span>';
        }
        var item_li = create_dropdown_tag_item(index, nodeid, nodename, defaultchecked);
        dropdown_tags_ul.appendChild(item_li);
    }

}

function create_dropdown_tag_item(tagindex, nodeid, nodename, defaultchecked)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", showtext:"+showtext);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", tagindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_tag_click(\""+ tagindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_tag_"+tagindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_tag_click(\""+ tagindex +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+nodename));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_tag_click(tagindex)
{
    glDropdown_checkbox_tag_click = true;
}

var glDropdown_checkbox_tag_click = false;
function dropdown_tag_click(tagindex)
{
    clear_table();
    // uncheck anothers
    var index;
    for	(index = 0; index < glTag_List.length; index++)
    {

        if ( index == tagindex)
        {
            nodeData = glTag_List[index];
            glCurrentTagMacaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            document.getElementById("dropdown_cur_tag").innerHTML = nodename + '<span class="caret"></span>';
            document.getElementById("dropdown_checkbox_tag_"+index).checked = true;
        }
        else
        {
            document.getElementById("dropdown_checkbox_tag_"+index).checked = false;
        }
    }

    glDropdown_checkbox_tag_click = false;
}

function clear_table()
{
    var diagnosis_table = document.getElementById('DIAGNOSIS_TABLE_CONTENT');

    while (diagnosis_table.rows.length >= 1)
       diagnosis_table.deleteRow(0);
}

//=============================================================================
// Locators
//
function put_locators_to_dropdownlist()
{
    var dropdown_locators_ul = document.getElementById("dropdown_locators");

    var index;
    for	(index = 0; index < glLocator_List.length; index++)
    {
        nodeData = glLocator_List[index];
        var macaddress = nodeData[6];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var LoRAMacaddress = nodeData[22];
        var defaultchecked = false;
        if (index == 0 )
        {
            glCurrentLocatorLoRAMacaddress = LoRAMacaddress;
            defaultchecked = true;
            document.getElementById("dropdown_cur_locator").innerHTML = nodename + '<span class="caret"></span>';
        }
        var item_li = create_dropdown_locator_item(index, nodename, defaultchecked);
        dropdown_locators_ul.appendChild(item_li);
    }
}

function create_dropdown_locator_item(locatorindex, nodename, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", locatorindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_locator_click(\""+ locatorindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_locator_"+locatorindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_locator_click(\""+ locatorindex +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+nodename));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_locator_click = false;
function dropdown_checkbox_locator_click(locatorindex)
{
    glDropdown_checkbox_locator_click = true;
}

function dropdown_locator_click(locatorindex)
{
    clear_locator_table();

    // uncheck anothers
    for	(var index = 0; index < glLocator_List.length; index++)
    {
        if ( index == locatorindex)
        {
            var nodeData = glLocator_List[index];
            var nodename = nodeData[5];
            var LoRAMacaddress = nodeData[22];
            glCurrentLocatorLoRAMacaddress = LoRAMacaddress;

            document.getElementById("dropdown_cur_locator").innerHTML = nodename + '<span class="caret"></span>';
            document.getElementById("dropdown_checkbox_locator_"+index).checked = true;
        }
        else
        {
            document.getElementById("dropdown_checkbox_locator_"+index).checked = false;
        }
    }

    glDropdown_checkbox_locator_click = false;
}

function clear_locator_table()
{
    var diagnosis_table = document.getElementById('LOCATOR_TABLE_CONTENT');

    while (diagnosis_table.rows.length >= 1)
       diagnosis_table.deleteRow(0);
}

//=============================================================================

function load_ranging_Diagnosis()
{
    document.getElementById("DIAGNOSIS_TABLE_MSG").innerText = "";

    if (glCurrentTagMacaddress.length == 0)
    {
        setTimeout(load_ranging_Diagnosis, 500);
        return true;
    }

    var targetURL = gl_target_server + "/php/diagnosis.php";
    var p = $.getJSON(targetURL, 'getrangingdiagnosis='+glCurrentTagMacaddress+'&project_id='+project_id, function(data)
    {
        var hasData = 0;

        var diagnosis_table = document.getElementById('DIAGNOSIS_TABLE_CONTENT');


        //while (diagnosis_table.rows.length >= 1)
        //    diagnosis_table.deleteRow(0);

        var data_count = 0;
        $.each(data, function(anchorkey, value)
        {
            hasData = 1;

            var infos = JSON.parse(value);

            var value_Ranging = infos["Ranging"];
            var value_PCnt = infos["PCnt"];
            var value_AnCnt = infos["AnCnt"];
            var value_TagRecv = infos["TagRecv"];
            var value_TagFP = infos["TagFP"];
            var value_AnRecv = infos["AnRecv"];
            var value_AnFP = infos["AnFP"];
            var value_TagVelocity = infos["TagVelocity"];
            var value_LostRate = infos["LostRate"];
            var value_SD = infos["SD"];
            var value_IMU = infos["IMU"];
            var value_DataRate = infos["DataRate"];
            var value_ValidRate = infos["ValidRate"];
            var value_DataCount = infos["DataCount"];
            var value_SlotTime = infos["SlotTime"];
            var value_ResetTime = infos["ResetTime"];

            if ( glAllAnchorsData[anchorkey] == undefined )
                glAllAnchorsData[anchorkey] = {};
            glAllAnchorsData[anchorkey]["Ranging"] = value_Ranging;
            glAllAnchorsData[anchorkey]["PCnt"] = value_PCnt;
            glAllAnchorsData[anchorkey]["AnCnt"] = value_AnCnt;
            glAllAnchorsData[anchorkey]["TagRecv"] = value_TagRecv;
            glAllAnchorsData[anchorkey]["TagFP"] = value_TagFP;
            glAllAnchorsData[anchorkey]["AnRecv"] = value_AnRecv;
            glAllAnchorsData[anchorkey]["AnFP"] = value_AnFP;
            glAllAnchorsData[anchorkey]["TagVelocity"] = value_TagVelocity;
            glAllAnchorsData[anchorkey]["LostRate"] = value_LostRate;
            glAllAnchorsData[anchorkey]["SD"] = value_SD;
            glAllAnchorsData[anchorkey]["IMU"] = value_IMU;
            glAllAnchorsData[anchorkey]["DataRate"] = value_DataRate;
            glAllAnchorsData[anchorkey]["ValidRate"] = value_ValidRate;
            glAllAnchorsData[anchorkey]["DataCount"] = value_DataCount;
            glAllAnchorsData[anchorkey]["SlotTime"] = value_SlotTime;
            glAllAnchorsData[anchorkey]["ResetTime"] = value_ResetTime;


            var row_anchor = document.getElementById('row_' + anchorkey);
            if ( row_anchor == null )
            {
                var row = create_table_row(glAnchorUIList.length, anchorkey, value_Ranging, value_PCnt,
                                value_AnCnt, value_TagRecv, value_TagFP, value_AnRecv, value_AnFP,
                                value_TagVelocity, value_LostRate, value_SD, value_IMU, value_DataRate, value_ValidRate,
                                value_DataCount, value_SlotTime, value_ResetTime);
                diagnosis_table.appendChild(row);
                glAnchorUIList.push(anchorkey);


            }
            else
            {
                // update info
                update_row(row_anchor, anchorkey, value_Ranging, value_PCnt,
                                value_AnCnt, value_TagRecv, value_TagFP, value_AnRecv, value_AnFP,
                                value_TagVelocity, value_LostRate, value_SD, value_IMU, value_DataRate, value_ValidRate,
                                value_DataCount, value_SlotTime, value_ResetTime);

            }
            data_count++;

            //calculate_acceleration(glCurrentTagMacaddress, infos["IMU"]);
        });

        update_timeout_table();

        $('.sparktristate').sparkline('html', {type: 'tristate'});
        // Bar charts using inline values
        $('.sparkbar').sparkline('html', {type: 'bar'});

        setTimeout(load_ranging_Diagnosis, 500);
    })
    .done( function()
    {
    //console.log("load_ranging_Diagnosis() done");
    })
    .fail( function (jqXHR, textStatus, errorThrown)
    {
        console.log("load_ranging_Diagnosis() fail");
        setTimeout(load_ranging_Diagnosis, 5000);
    })
    .always( function()
    {
    });

    //setTimeout(function(){ p.abort(); }, 10000);

    return false;
}

function load_locator_rssiinfo()
{
    if (glCurrentLocatorLoRAMacaddress.length == 0)
    {
        setTimeout(load_locator_rssiinfo, 1000);
        return true;
    }

    var targetURL = gl_target_server + "/php/diagnosis.php";
    var p = $.getJSON(targetURL, 'getlocatorrssiinfo='+glCurrentLocatorLoRAMacaddress+'&project_id='+project_id, function(data)
    {
        var diagnosis_table = document.getElementById('LOCATOR_TABLE_CONTENT');

        var data_count = 0;
        $.each(data, function(gwid, value)
        {
            var infos = JSON.parse(value);

            var value_rssi = infos["rssi"];
            var value_count = infos["count"];
            var value_startdatetime = infos["startdatetime"];
            var value_enddatetime = infos["enddatetime"];

            if ( glAllGatewayData[gwid] == undefined )
                glAllGatewayData[gwid] = {};

            glAllGatewayData[gwid]["rssi"] = value_rssi;
            glAllGatewayData[gwid]["count"] = value_count;
            glAllGatewayData[gwid]["startdatetime"] = value_startdatetime;
            glAllGatewayData[gwid]["enddatetime"] = value_enddatetime;

            var row_gateway = document.getElementById('row_' + gwid);
            if ( row_gateway == undefined )
            {
                var row = create_gateway_table_row(data_count, gwid, value_rssi, value_count, value_startdatetime, value_enddatetime);
                diagnosis_table.appendChild(row);
                // glGatewayUIList.push(gwid);
            }
            else
            {
                // update info
                update_gateway_row(row_gateway, gwid, value_rssi, value_count, value_startdatetime, value_enddatetime);
            }
            data_count++;
        });

        setTimeout(load_locator_rssiinfo, 1000);
    })
    .done( function()
    {
    })
    .fail( function (jqXHR, textStatus, errorThrown)
    {
        console.log("load_locator_rssiinfo() fail");
        setTimeout(load_locator_rssiinfo, 5000);
    })
    .always( function()
    {
    });

    return false;
}

function create_table_row(ui_index, anchor, Ranging, TagPollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm,
                        AnchorRecvdBm, AnchorFirstPathdBm, TagVelocity, LostRate, SD, IMU, DataRate, ValidRate, DataCount, SlotTime, ResetTime)
{
    var cellIndex = 0;

    var numDataRate = Number(DataRate);
    var numValidRate = Number(ValidRate);
    var numResetTime = Number(ResetTime);
    var numTargetDataRate = 1000.0/numResetTime;

    var row = document.createElement('tr');
    row.setAttribute('id', 'row_' + anchor);

    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // anchor
    row.appendChild(document.createElement('td'));
    var SpanAnchor = document.createElement("span");
    SpanAnchor.setAttribute("class","badge bg-light-blue");
    SpanAnchor.textContent = getAnchorName(anchor);
    row.cells[cellIndex].appendChild(SpanAnchor);
    cellIndex++;

    // Ranging
    var RangingNumber = Number(Ranging);
    row.appendChild(document.createElement('td'));
    var SpanRanging = document.createElement("span");
    SpanRanging.setAttribute('id', 'row_' + anchor + '_Ranging');
    if (RangingNumber >= 2500)
        SpanRanging.setAttribute("class","badge bg-red");
    else
        SpanRanging.setAttribute("class","badge bg-yellow");
    SpanRanging.textContent = Ranging;
    row.cells[cellIndex].appendChild(SpanRanging);
    cellIndex++;

    // Standard Deviation
    var sdNumber = Number(SD);
    row.appendChild(document.createElement('td'));
    var SpanSD = document.createElement("span");
    SpanSD.setAttribute('id', 'row_' + anchor + '_SD');
    if (sdNumber >= 8.0)
        SpanSD.setAttribute("class","badge bg-red");
    else
        SpanSD.setAttribute("class","badge bg-green");
    SpanSD.textContent = SD;
    row.cells[cellIndex].appendChild(SpanSD);
    cellIndex++;

    // Loat Rate
    var LostRatemNumber = Number(LostRate);
    row.appendChild(document.createElement('td'));
    var DivLostRate = document.createElement("div");
    DivLostRate.setAttribute("class","progress progress-xs");
    var DivLostRate1 = document.createElement("div");
    DivLostRate1.setAttribute('id', 'row_' + anchor + '_LostRate1');
    if (LostRatemNumber >= 10)
        DivLostRate1.setAttribute("class","progress-bar progress-bar-danger");
    else
        DivLostRate1.setAttribute("class","progress-bar progress-bar-green");
    DivLostRate1.setAttribute("style","width:"+LostRate+"%");
    DivLostRate.appendChild(DivLostRate1);
    row.cells[cellIndex].appendChild(DivLostRate);
    var SpanLostRate = document.createElement("span");
    SpanLostRate.setAttribute('id', 'row_' + anchor + '_LostRate');
    if (LostRatemNumber >= 10)
        SpanLostRate.setAttribute("class","badge bg-red");
    else
        SpanLostRate.setAttribute("class","badge bg-green");
    SpanLostRate.textContent = LostRate;
    row.cells[cellIndex].appendChild(SpanLostRate);
    cellIndex++;

    // Data Rate
    row.appendChild(document.createElement('td'));
    var SpanDataRate = document.createElement("span");
    SpanDataRate.setAttribute('id', 'row_' + anchor + '_DataRate');
    //SpanDataRate.setAttribute("class","badge bg-green");
    if (numDataRate < numTargetDataRate)
        SpanDataRate.setAttribute("class","badge bg-red");
    else
        SpanDataRate.setAttribute("class","badge bg-green");
    SpanDataRate.textContent = Number(DataRate).toFixed(1);
    row.cells[cellIndex].appendChild(SpanDataRate);

    var SpanValidRate = document.createElement("span");
    SpanValidRate.setAttribute('id', 'row_' + anchor + '_ValidRate');
    //SpanDataRate.setAttribute("class","badge bg-green");
    if (numValidRate < 1.0)
        SpanValidRate.setAttribute("class","badge bg-red");
    else
        SpanValidRate.setAttribute("class","badge bg-green");
    SpanValidRate.textContent = Number(ValidRate).toFixed(1);
    row.cells[cellIndex].appendChild(SpanValidRate);


    cellIndex++;

    // Data Count
    row.appendChild(document.createElement('td'));
    var SpanDataCount = document.createElement("span");
    SpanDataCount.setAttribute('id', 'row_' + anchor + '_DataCount');
    SpanDataCount.setAttribute("class","badge bg-green");
    SpanDataCount.textContent = Number(DataCount);
    row.cells[cellIndex].appendChild(SpanDataCount);
    cellIndex++;

    // SlotTime/ResetTime
    row.appendChild(document.createElement('td'));
    var SpanSlotTime = document.createElement("span");
    SpanSlotTime.setAttribute('id', 'row_' + anchor + '_SlotTime');
    SpanSlotTime.setAttribute("class","badge bg-yellow");
    SpanSlotTime.textContent = Number(SlotTime);
    row.cells[cellIndex].appendChild(SpanSlotTime);
    var SpanResetTime = document.createElement("span");
    SpanResetTime.setAttribute('id', 'row_' + anchor + '_ResetTime');
    SpanResetTime.setAttribute("class","badge bg-blue");
    SpanResetTime.textContent = Number(ResetTime);
    row.cells[cellIndex].appendChild(SpanResetTime);
    var SpanRangingDataRate = document.createElement("span");
    SpanRangingDataRate.setAttribute('id', 'row_' + anchor + '_RangingDataRate');
    SpanRangingDataRate.setAttribute("class","badge bg-light-blue");
    SpanRangingDataRate.textContent = Number(numTargetDataRate).toFixed(2);
    row.cells[cellIndex].appendChild(SpanRangingDataRate);
    cellIndex++;

    // // Poll count
    // row.appendChild(document.createElement('td'));
    // var SpanPollCnt = document.createElement("span");
    // SpanPollCnt.setAttribute('id', 'row_' + anchor + '_PollCnt');
    // SpanPollCnt.setAttribute("class","badge bg-green");
    // SpanPollCnt.textContent = TagPollCnt;
    // row.cells[cellIndex].appendChild(SpanPollCnt);
    // cellIndex++;

    // dBm
    row.appendChild(document.createElement('td'));

    // recvdBm
    var recvdBmNumber = Number(TagFinalRecvdBm);
    var SpanrecvdBm = document.createElement("span");
    SpanrecvdBm.setAttribute('id', 'row_' + anchor + '_recvdBm');
    if (recvdBmNumber <= -135.0)
        SpanrecvdBm.setAttribute("class","badge bg-red");
    else
        SpanrecvdBm.setAttribute("class","badge bg-green");
    SpanrecvdBm.textContent = TagFinalRecvdBm;
    row.cells[cellIndex].appendChild(SpanrecvdBm);

    // firstpathdBm
    var firstpathdBmNumber = Number(TagFirstPathdBm);
    var SpanfirstpathdBm = document.createElement("span");
    SpanfirstpathdBm.setAttribute('id', 'row_' + anchor + '_firstpathdBm');
    if (recvdBmNumber <= -135.0)
        SpanfirstpathdBm.setAttribute("class","badge bg-red");
    else
        SpanfirstpathdBm.setAttribute("class","badge bg-green");
    SpanfirstpathdBm.textContent = TagFirstPathdBm;
    row.cells[cellIndex].appendChild(SpanfirstpathdBm);

    // Anchor recvdBm
    var anrecvdBmNumber = Number(AnchorRecvdBm);
    var SpananrecvdBm = document.createElement("span");
    SpananrecvdBm.setAttribute('id', 'row_' + anchor + '_anrecvdBm');
    if (anrecvdBmNumber <= -135.0)
        SpananrecvdBm.setAttribute("class","badge bg-red");
    else
        SpananrecvdBm.setAttribute("class","badge bg-green");
    SpananrecvdBm.textContent = AnchorRecvdBm;
    row.cells[cellIndex].appendChild(SpananrecvdBm);

    //  Anchor firstpathdBm
    var anfirstpathdBmNumber = Number(AnchorFirstPathdBm);
    var SpananfirstpathdBm = document.createElement("span");
    SpananfirstpathdBm.setAttribute('id', 'row_' + anchor + '_anfirstpathdBm');
    if (anfirstpathdBmNumber <= -135.0)
        SpananfirstpathdBm.setAttribute("class","badge bg-red");
    else
        SpananfirstpathdBm.setAttribute("class","badge bg-green");
    SpananfirstpathdBm.textContent = AnchorFirstPathdBm;
    row.cells[cellIndex].appendChild(SpananfirstpathdBm);

    cellIndex++;

    // TagVelocity
    row.appendChild(document.createElement('td'));
    var SpanTagVelocity = document.createElement("span");
    SpanTagVelocity.setAttribute('id', 'row_' + anchor + '_TagVelocity');
    SpanTagVelocity.setAttribute("class","badge bg-green");
    SpanTagVelocity.textContent = TagVelocity;
    row.cells[cellIndex].appendChild(SpanTagVelocity);
    cellIndex++;

    // Ranging Anchor count
    row.appendChild(document.createElement('td'));
    var SpanRangingAnchorCnt = document.createElement("span");
    SpanRangingAnchorCnt.setAttribute('id', 'row_' + anchor + '_RangingAnchorCnt');
    SpanRangingAnchorCnt.setAttribute("class","badge bg-green");
    SpanRangingAnchorCnt.textContent = RangingAnchorCnt;
    row.cells[cellIndex].appendChild(SpanRangingAnchorCnt);
    cellIndex++;

    // IMU
    row.appendChild(document.createElement('td'));
    var SpanIMU = document.createElement("span");
    SpanIMU.setAttribute('id', 'row_' + anchor + '_IMU');
    SpanIMU.setAttribute("class","sparkbar");
    SpanIMU.textContent = IMU;
    row.cells[cellIndex].appendChild(SpanIMU);
    cellIndex++;

    return row;
}

function update_row(row_anchor, anchor, Ranging, TagPollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm,
                        AnchorRecvdBm, AnchorFirstPathdBm, TagVelocity, LostRate, SD, IMU, DataRate, ValidRate, DataCount, SlotTime, ResetTime)
{
    var row_anchor_ranging = document.getElementById('row_' + anchor + '_Ranging');
    var RangingNumber = Number(Ranging);
    if (RangingNumber >= 2500)
        row_anchor_ranging.setAttribute("class","badge bg-red");
    else
        row_anchor_ranging.setAttribute("class","badge bg-yellow");
    row_anchor_ranging.textContent = Ranging;

    var row_anchor_SD = document.getElementById('row_' + anchor + '_SD');
    var sdNumber = Number(SD);
    if (sdNumber >= 8.0)
        row_anchor_SD.setAttribute("class","badge bg-red");
    else
        row_anchor_SD.setAttribute("class","badge bg-green");
    row_anchor_SD.textContent = SD;

    var row_anchor_LostRate1 = document.getElementById('row_' + anchor + '_LostRate1');
    var LostRatemNumber = Number(LostRate);
    if (LostRatemNumber >= 10)
        row_anchor_LostRate1.setAttribute("class","progress-bar progress-bar-danger");
    else
        row_anchor_LostRate1.setAttribute("class","progress-bar progress-bar-green");
    row_anchor_LostRate1.setAttribute("style","width:"+LostRate+"%");

    var row_anchor_LostRate = document.getElementById('row_' + anchor + '_LostRate');
    if (LostRatemNumber >= 10)
        row_anchor_LostRate.setAttribute("class","badge bg-red");
    else
        row_anchor_LostRate.setAttribute("class","badge bg-green");
    row_anchor_LostRate.textContent = LostRate;

    // var row_anchor_PollCnt = document.getElementById('row_' + anchor + '_PollCnt');
    // row_anchor_PollCnt.textContent = TagPollCnt;

    var numDataRate = Number(DataRate);
    var numValidRate = Number(ValidRate);
    var numResetTime = Number(ResetTime);
    var numTargetDataRate = 1000.0/numResetTime;

    var row_anchor_DataRate = document.getElementById('row_' + anchor + '_DataRate');
    if (numDataRate < numTargetDataRate)
        row_anchor_DataRate.setAttribute("class","badge bg-red");
    else
        row_anchor_DataRate.setAttribute("class","badge bg-green");
    row_anchor_DataRate.textContent = Number(DataRate).toFixed(1);

    var row_anchor_ValidRate = document.getElementById('row_' + anchor + '_ValidRate');
    if (numValidRate < 1.0)
        row_anchor_ValidRate.setAttribute("class","badge bg-red");
    else
        row_anchor_ValidRate.setAttribute("class","badge bg-green");
    row_anchor_ValidRate.textContent = Number(ValidRate).toFixed(1);


    var row_anchor_DataCount = document.getElementById('row_' + anchor + '_DataCount');
    row_anchor_DataCount.textContent = Number(DataCount);

    var row_anchor_SlotTime = document.getElementById('row_' + anchor + '_SlotTime');
    row_anchor_SlotTime.textContent = Number(SlotTime);

    var row_anchor_ResetTime = document.getElementById('row_' + anchor + '_ResetTime');
    row_anchor_ResetTime.textContent = Number(ResetTime);

    var row_anchor_RangingDataRate = document.getElementById('row_' + anchor + '_RangingDataRate');
    row_anchor_RangingDataRate.textContent = Number(numTargetDataRate).toFixed(2);

    var row_anchor_recvdBm = document.getElementById('row_' + anchor + '_recvdBm');
    var recvdBmNumber = Number(TagFinalRecvdBm);
    if (recvdBmNumber <= -135.0)
        row_anchor_recvdBm.setAttribute("class","badge bg-red");
    else
        row_anchor_recvdBm.setAttribute("class","badge bg-green");
    row_anchor_recvdBm.textContent = TagFinalRecvdBm;


    var row_anchor_firstpathdBm = document.getElementById('row_' + anchor + '_firstpathdBm');
    var firstpathdBmNumber = Number(TagFirstPathdBm);
    if (recvdBmNumber <= -135.0)
        row_anchor_firstpathdBm.setAttribute("class","badge bg-red");
    else
        row_anchor_firstpathdBm.setAttribute("class","badge bg-green");
    row_anchor_firstpathdBm.textContent = TagFirstPathdBm;

    // Anchor recvdBm
    var row_anchor_anrecvdBm = document.getElementById('row_' + anchor + '_anrecvdBm');
    var anrecvdBmNumber = Number(AnchorRecvdBm);
    if (anrecvdBmNumber <= -135.0)
        row_anchor_anrecvdBm.setAttribute("class","badge bg-red");
    else
        row_anchor_anrecvdBm.setAttribute("class","badge bg-green");
    row_anchor_anrecvdBm.textContent = AnchorRecvdBm;

    //  Anchor firstpathdBm
    var row_anchor_anrecvdBm = document.getElementById('row_' + anchor + '_anfirstpathdBm');
    var anfirstpathdBmNumber = Number(AnchorFirstPathdBm);
     if (anfirstpathdBmNumber <= -135.0)
        row_anchor_anrecvdBm.setAttribute("class","badge bg-red");
    else
        row_anchor_anrecvdBm.setAttribute("class","badge bg-green");
    row_anchor_anrecvdBm.textContent = AnchorFirstPathdBm;


    // TagVelocity
    var row_anchor_TagVelocity = document.getElementById('row_' + anchor + '_TagVelocity');
    row_anchor_TagVelocity.textContent = TagVelocity;

    // Ranging Anchor count
    var row_anchor_RangingAnchorCnt = document.getElementById('row_' + anchor + '_RangingAnchorCnt');
    row_anchor_RangingAnchorCnt.textContent = RangingAnchorCnt;

    // IMU
    var row_anchor_IMU = document.getElementById('row_' + anchor + '_IMU');
    row_anchor_IMU.textContent = IMU;
}

function create_gateway_table_row(ui_index, gwid, rssi, count, startdatetime, enddatetime)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute('id', 'row_' + gwid);

    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Gateway
    row.appendChild(document.createElement('td'));
    var SpanGateway = document.createElement("span");
    SpanGateway.setAttribute("class","badge bg-light-blue");
    SpanGateway.textContent = gwid;
    row.cells[cellIndex].appendChild(SpanGateway);
    cellIndex++;

    // RSSI
    row.appendChild(document.createElement('td'));
    var SpanRSSI = document.createElement("span");
    SpanRSSI.setAttribute('id', 'row_' + gwid + '_RSSI');
    SpanRSSI.setAttribute("class","badge bg-red");
    SpanRSSI.textContent = rssi;
    row.cells[cellIndex].appendChild(SpanRSSI);
    cellIndex++;

    // Count
    row.appendChild(document.createElement('td'));
    var SpanCount = document.createElement("span");
    SpanCount.setAttribute('id', 'row_' + gwid + '_Count');
    SpanCount.setAttribute("class","badge bg-light-green");
    SpanCount.textContent = count;
    row.cells[cellIndex].appendChild(SpanCount);
    cellIndex++;

    // Start time
    row.appendChild(document.createElement('td'));
    var badge_StartDate = document.createElement("span");
    badge_StartDate.setAttribute('id', 'row_' + gwid + '_StartDate');
    badge_StartDate.setAttribute("class","label label-info");
    badge_StartDate.textContent = convertPrettyDateOnly(startdatetime);
    row.cells[cellIndex].appendChild(badge_StartDate);

    var badge_StartTime = document.createElement("span");
    badge_StartTime.setAttribute('id', 'row_' + gwid + '_StartTime');
    badge_StartTime.setAttribute("class","label label-warning");
    badge_StartTime.textContent = convertPrettyTimeOnly(startdatetime);
    row.cells[cellIndex].appendChild(badge_StartTime);
    cellIndex++;

    // row.appendChild(document.createElement('td'));
    // var SpanStartTime = document.createElement("span");
    // SpanStartTime.setAttribute('id', 'row_' + gwid + '_StartTime');
    // SpanStartTime.setAttribute("class","badge bg-light-blue");
    // SpanStartTime.textContent = startdatetime;
    // row.cells[cellIndex].appendChild(SpanStartTime);
    // cellIndex++;

    // End time
    row.appendChild(document.createElement('td'));
    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute('id', 'row_' + gwid + '_EndDate');
    badge_EndDate.setAttribute("class","label label-info");
    badge_EndDate.textContent = convertPrettyDateOnly(enddatetime);
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute('id', 'row_' + gwid + '_EndTime');
    badge_EndTime.setAttribute("class","label label-warning");
    badge_EndTime.textContent = convertPrettyTimeOnly(enddatetime);
    row.cells[cellIndex].appendChild(badge_EndTime);
    cellIndex++;

    // row.appendChild(document.createElement('td'));
    // var SpanEndTime = document.createElement("span");
    // SpanEndTime.setAttribute('id', 'row_' + gwid + '_EndTime');
    // SpanEndTime.setAttribute("class","badge bg-light-blue");
    // SpanEndTime.textContent = enddatetime;
    // row.cells[cellIndex].appendChild(SpanEndTime);
    // cellIndex++;

    return row;
}

function update_gateway_row(row_gateway, gwid, rssi, count, startdatetime, enddatetime)
{
    var row_gateway_RSSI = document.getElementById('row_' + gwid + '_RSSI');
    var row_gateway_Count = document.getElementById('row_' + gwid + '_Count');
    var row_gateway_StartDate = document.getElementById('row_' + gwid + '_StartDate');
    var row_gateway_StartTime = document.getElementById('row_' + gwid + '_StartTime');
    var row_gateway_EndDate = document.getElementById('row_' + gwid + '_EndDate');
    var row_gateway_EndTime = document.getElementById('row_' + gwid + '_EndTime');

    row_gateway_RSSI.textContent = rssi;
    row_gateway_Count.textContent = count;
    row_gateway_StartDate.textContent = convertPrettyDateOnly(startdatetime);
    row_gateway_StartTime.textContent = convertPrettyTimeOnly(startdatetime);
    row_gateway_EndDate.textContent = convertPrettyDateOnly(enddatetime);
    row_gateway_EndTime.textContent = convertPrettyTimeOnly(enddatetime);

}

function update_timeout_table()
{
    $.each(glAllAnchorsData, function(anchorkey, infos)
    {
        //console.log("update_timeout_table() anchorkey:"+ anchorkey + ", Ranging:"+infos["Ranging"]);

        var newCount = 0;
        if(glAllAnchorsPreData[anchorkey] == undefined)
        {
            glAllAnchorsPreData[anchorkey] = {};

            newCount = 15;
        }
        else
        {
            //
            // check new data
            //
            if ( glAllAnchorsPreData[anchorkey]['Ranging'] != infos["Ranging"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['PCnt'] != infos["PCnt"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['AnCnt'] != infos["AnCnt"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['TagRecv'] != infos["TagRecv"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['TagFP'] != infos["TagFP"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['AnRecv'] != infos["AnRecv"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['AnFP'] != infos["AnFP"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['TagVelocity'] != infos["TagVelocity"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['LostRate'] != infos["LostRate"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['SD'] != infos["SD"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['IMU'] != infos["IMU"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['DataRate'] != infos["DataRate"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['ValidRate'] != infos["ValidRate"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['DataCount'] != infos["DataCount"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['SlotTime'] != infos["SlotTime"] )
                newCount++;
            if ( glAllAnchorsPreData[anchorkey]['ResetTime'] != infos["ResetTime"] )
                newCount++;
        }
        glAllAnchorsPreData[anchorkey]["Ranging"] = infos["Ranging"];
        glAllAnchorsPreData[anchorkey]["PCnt"] = infos["PCnt"];
        glAllAnchorsPreData[anchorkey]["AnCnt"] = infos["AnCnt"];
        glAllAnchorsPreData[anchorkey]["TagRecv"] = infos["TagRecv"];
        glAllAnchorsPreData[anchorkey]["TagFP"] = infos["TagFP"];
        glAllAnchorsPreData[anchorkey]["AnRecv"] = infos["AnRecv"];
        glAllAnchorsPreData[anchorkey]["AnFP"] = infos["AnFP"];
        glAllAnchorsPreData[anchorkey]["TagVelocity"] = infos["TagVelocity"];
        glAllAnchorsPreData[anchorkey]["LostRate"] = infos["LostRate"];
        glAllAnchorsPreData[anchorkey]["SD"] = infos["SD"];
        glAllAnchorsPreData[anchorkey]["IMU"] = infos["IMU"];
        glAllAnchorsPreData[anchorkey]["DataRate"] = infos["DataRate"];
        glAllAnchorsPreData[anchorkey]["ValidRate"] = infos["ValidRate"];
        glAllAnchorsPreData[anchorkey]["DataCount"] = infos["DataCount"];
        glAllAnchorsPreData[anchorkey]["SlotTime"] = infos["SlotTime"];
        glAllAnchorsPreData[anchorkey]["ResetTime"] = infos["ResetTime"];

        var NoNewDataValid = 0;
        if (glAllAnchorsUpdateTime[anchorkey] == undefined)
        {
            glAllAnchorsUpdateTime[anchorkey] = new Date();
        }
        else
        {
            //console.log(anchorkey + " newCount:"+newCount +", NoNewDataValid:"+NoNewDataValid);

            if (newCount > 0)
            {
                glAllAnchorsUpdateTime[anchorkey] = new Date();
            }
            else
            {
                //
                // check timeout
                //
                var previousTime = glAllAnchorsUpdateTime[anchorkey];
                var currentTime = new Date();

                var diffTime = getDifferentTimeInSeconds(previousTime, currentTime);
                if (diffTime > 3)
                {
                    NoNewDataValid = 1;
                }
                //console.log(anchorkey + " diffTime:"+diffTime +", NoNewDataValid:"+NoNewDataValid);
            }
        }
        var row_anchor = document.getElementById('row_' + anchorkey);
        if ( row_anchor != null )
        {
            if (NoNewDataValid == 1)
            {
                row_anchor.setAttribute("bgcolor","#a3c2c2");
            }
            else
            {
                row_anchor.setAttribute("bgcolor","#FFFFFF");
            }
        }
    });
}

function calculate_acceleration(nodemac, IMU)
{
    var imuData = IMU.split(",");
    var aX = Number(imuData[0]);
    var aY = Number(imuData[1]);
    var aZ = Number(imuData[2]);

    var acceleration = Math.sqrt(aX * aX + aY * aY + aZ * aZ);

    console.log(nodemac+" IMU:"+IMU +" ("+aX+","+aY+","+aZ+") acceleration:"+acceleration);
}

function getDifferentTimeInSeconds(previousTime, currentTime)
{
    var dif = currentTime.getTime() - previousTime.getTime();

    var Seconds_from_T1_to_T2 = dif / 1000;
    var Seconds_Between_Dates = Math.abs(Seconds_from_T1_to_T2);

    return Seconds_Between_Dates;
}

function getTagName(nodeid)
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;

    }
    return nodeid;
}

function getAnchorName(nodeid)
{
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;
    }

    for (var i = 0; i < glAnchor_List.length; i++)
    {
        var nodeData = glAnchor_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;
    }

    return nodeid;
}

function convertPrettyDateOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";
    if (seconds >= 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function open_diagnosis_page()
{
    var win = window.open('/index.php/vilsdiagnosis/'+project_id, '_blank');
    win.focus();
}
