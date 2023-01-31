/*
 * Author: ikki Chung
 * Date: 26 Feb 2020
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var glTag_List = [];
var glUnknown_pos_Tag_List = [];
var glNonAliveTag_List = [];

var glFetch_unknown_poistion_count = 0;

function fetch_unknown_poistion_tag()
{
    if (glFetch_unknown_poistion_count == 0 )
        glUnknown_pos_Tag_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadunknownpositiontag=1&project_id='+project_id, function(data)
    {
        var count = 0;
        $.each(data, function(curindex, nodeinfo)
        {

            // console.log("curindex:"+curindex+" macaddress:"+nodeinfo.macaddress+" pos:"+nodeinfo.pos+" glTag_List.length:"+glTag_List.length);

            for (var i = 0; i < glTag_List.length; ++i)
            {
                var nodeData = glTag_List[i];
                var node_macaddress = nodeData[6];
                if( node_macaddress === nodeinfo.macaddress )
                {
                    var pos = nodeinfo.pos.split(",");

                    // check existed
                    var bFound = false;
                    for (var j = 0; j < glUnknown_pos_Tag_List.length; ++j)
                    {
                        var existed = glUnknown_pos_Tag_List[j];
                        var existed_macaddress = existed[6];
                        if( node_macaddress === existed_macaddress )
                        {
                            nodeData[100] = pos[0];
                            nodeData[101] = pos[1];
                            nodeData[102] = pos[2];

                            bFound = true;
                            break;
                        }
                    }
                    if (bFound == false)
                    {
                        nodeData[100] = pos[0];
                        nodeData[101] = pos[1];
                        nodeData[102] = pos[2];
                        glUnknown_pos_Tag_List.push(nodeData);
                    }
                }
            }

            count++;
        });


        // var tag_num = parseInt(data['LOC_TAG_NUM']);
        //
        // for (var i = 0; i < tag_num; ++i)
        // {
        //     var key = 'LOC_TAG_INDEX_'+i;
        //     if (data[key] == undefined || data[key] == null)
        //         continue;
        //     // split data
        //     var nodeData = data[key].split(",");
        //
        //     glUnknown_pos_Tag_List.push(nodeData);
        // }

    })
    .success(function() {
        show_unknown_poistion_tag();

        if (glFetch_unknown_poistion_count < 5)
            setTimeout(fetch_unknown_poistion_tag, 500);

        glFetch_unknown_poistion_count++;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function fetch_NonAlive_tag()
{
    glNonAliveTag_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        for (var i = 0; i < tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            // split data
            var nodeData = data[key].split(",");
            var alive = nodeData[1];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var macaddress = nodeData[6];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var asleep = parseInt(nodeData[35]);

            if( alive === "0" )
            {
                glNonAliveTag_List.push(nodeData);
            }
        }

    })
    .success(function() {
        show_non_alive_tag();
    })
    .error(function() {
    })
    .complete(function() {
    });

}

function fetch_all_tag()
{
    glTag_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        for (var i = 0; i < tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            // split data
            var nodeData = data[key].split(",");
            var alive = nodeData[1];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var macaddress = nodeData[6];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var asleep = parseInt(nodeData[35]);

            glTag_List.push(nodeData);

        }

    })
    .success(function() {

        setTimeout(fetch_unknown_poistion_tag, 10);

    })
    .error(function() {
    })
    .complete(function() {
    });

}

function FixPosition()
{
    $('#openfixpositionmodalbtn').click();

    glFetch_unknown_poistion_count = 0;

    fetch_all_tag();

}

function show_unknown_poistion_tag()
{
    var tabl_body = document.getElementById('tag_list_content_fixposition');

    while (tabl_body.firstChild)
    {
        tabl_body.removeChild(tabl_body.firstChild);
    }

    // console.log("glUnknown_pos_Tag_List.length:"+glUnknown_pos_Tag_List.length);

    for (var i = 0; i < glUnknown_pos_Tag_List.length; ++i)
    {
        var nodeData = glUnknown_pos_Tag_List[i];
        var alive = nodeData[1];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var macaddress = nodeData[6];
        var posX = parseInt(nodeData[7]);
        var posY = parseInt(nodeData[8]);
        var posZ = parseInt(nodeData[9]);
        var asleep = parseInt(nodeData[35]);

        var posXraw = parseInt(nodeData[100]);
        var posYraw = parseInt(nodeData[101]);
        var posZraw = parseInt(nodeData[102]);

        var diff_x = posXraw - posX;
        var diff_y = posYraw - posY;
        var diff_z = posYraw - posZ;

        var diff_dist = parseInt(Math.sqrt( diff_x*diff_x + diff_y*diff_y + diff_z*diff_z));

        // console.log("diff_dist:"+diff_dist);

        var row = create_fixposition_tag_status_row(i+1, nodename, macaddress, diff_dist);
        tabl_body.appendChild(row);
    }
}

function create_fixposition_tag_status_row(ui_index, nodename, macaddress, diff_dist)
{
    var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("class","badge bg-light-blue-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    cellIndex++;

    // diff_dist
    row.appendChild(document.createElement('td'));
    var span_diff_dist = document.createElement("span");
    span_diff_dist.setAttribute("class","badge bg-fuchsia");
    span_diff_dist.textContent = diff_dist;
    row.cells[cellIndex].appendChild(span_diff_dist);
    cellIndex++;

    row.appendChild(document.createElement('td'));
    var div_form_group = document.createElement("div");
    div_form_group.setAttribute("class","form-group");
    row.cells[cellIndex].appendChild(div_form_group);

    // radio_1
    var div_radio_1 = document.createElement("div");
    div_radio_1.setAttribute("class","radio");
    div_form_group.appendChild(div_radio_1);

    var div_radio_1_label = document.createElement("label");
    div_radio_1.appendChild(div_radio_1_label);

    var div_radio_1_label_input = document.createElement("input");
    div_radio_1_label_input.setAttribute("type","radio");
    div_radio_1_label_input.setAttribute("name","optionRadio_"+macaddress);
    div_radio_1_label_input.setAttribute("id","optionRadio1_"+macaddress);
    div_radio_1_label_input.setAttribute("value","option1");
    div_radio_1_label_input.setAttribute("checked","true");
    //div_radio_1_label_input.setAttribute("onclick","optionRadio_onclick(1, \""+macaddress+"\");");
    div_radio_1_label.appendChild(div_radio_1_label_input);
    div_radio_1_label.appendChild(document.createTextNode("維持"));


    // radio_2
    var div_radio_2 = document.createElement("div");
    div_radio_2.setAttribute("class","radio");
    div_form_group.appendChild(div_radio_2);

    var div_radio_2_label = document.createElement("label");
    div_radio_2.appendChild(div_radio_2_label);

    var div_radio_2_label_input = document.createElement("input");
    div_radio_2_label_input.setAttribute("type","radio");
    div_radio_2_label_input.setAttribute("name","optionRadio_"+macaddress);
    div_radio_2_label_input.setAttribute("id","optionRadio2_"+macaddress);
    div_radio_2_label_input.setAttribute("value","option2");
    //div_radio_2_label_input.setAttribute("onclick","optionRadio_onclick(2, \""+macaddress+"\");");
    div_radio_2_label.appendChild(div_radio_2_label_input);
    div_radio_2_label.appendChild(document.createTextNode("移動(至最新坐標)"));

    cellIndex++;

    return row;
}

function fixposition_update()
{
    document.getElementById("fixposition_updatebtn").disabled = true;
    document.getElementById("fixposition_sending_callout").style.display = 'block';
    document.getElementById("fixposition_failed_callout").style.display = 'none';
    document.getElementById("fixposition_finished_callout").style.display = 'none';

    var updateNode = [];

    for (var i = 0; i < glUnknown_pos_Tag_List.length; ++i)
    {
        var nodeData = glUnknown_pos_Tag_List[i];
        var macaddress = nodeData[6];

        var radios = document.getElementsByName("optionRadio_"+macaddress);
        if (radios[0].checked)
        {
            console.log(macaddress + ":" + radios[0].value);
        }
        else
        if (radios[1].checked)
        {
            console.log(macaddress + ":" + radios[1].value);
            updateNode.push(macaddress);
        }
    }

    if (updateNode.length == 0)
    {
        document.getElementById("fixposition_sending_callout").style.display = 'none';
        document.getElementById("fixposition_failed_callout").style.display = 'none';
        document.getElementById("fixposition_finished_callout").style.display = 'none';

        document.getElementById("fixposition_reponse_msg_callout").style.display = 'block';
        document.getElementById("fixposition_reponse_msg_callout").innerHTML = "<p>沒有選擇裝置更新!</p>"

        setTimeout(close_callout, 5000, "fixposition_reponse_msg_callout");
        document.getElementById("fixposition_updatebtn").disabled = false;
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['fixposition'] = '1';
    jsondata['updatenode'] = updateNode;

    jQuery.ajax({
        url: targetURL,
        type: 'POST',
        data: jsondata,
        success:
            function(response)
            {
                document.getElementById("fixposition_sending_callout").style.display = 'none';
                document.getElementById("fixposition_failed_callout").style.display = 'none';
                document.getElementById("fixposition_finished_callout").style.display = 'block';

                setTimeout(close_callout, 5000, "fixposition_finished_callout");
                document.getElementById("fixposition_updatebtn").disabled = false;
            },
        error:
            function(xhr, status, error)
            {
                //alert("error = " + error);
                //alert("xhr.responseText = " + xhr.responseText);
                document.getElementById("fixposition_sending_callout").style.display = 'none';
                document.getElementById("fixposition_failed_callout").style.display = 'block';
                document.getElementById("fixposition_finished_callout").style.display = 'none';

                setTimeout(close_callout, 5000, "fixposition_failed_callout");
                document.getElementById("fixposition_updatebtn").disabled = false;
            }
    });

}

function FixDevice()
{
    $('#openfixdevicemodalbtn').click();

    fetch_NonAlive_tag();

}

function show_non_alive_tag()
{
    var tabl_body = document.getElementById('tag_list_content_fixdevice');

    while (tabl_body.firstChild)
    {
        tabl_body.removeChild(tabl_body.firstChild);
    }

    for (var i = 0; i < glNonAliveTag_List.length; ++i)
    {
        var nodeData = glNonAliveTag_List[i];
        // var alive = nodeData[1];
        // var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var macaddress = nodeData[6];
        // var posX = nodeData[7];
        // var posY = nodeData[8];
        // var posZ = nodeData[9];
        var asleep = nodeData[35];

        // var diff_dist = 51 + i;

        var row = create_fixdevice_tag_status_row(i+1, nodename, macaddress, asleep);
        tabl_body.appendChild(row);
    }
}

function create_fixdevice_tag_status_row(ui_index, nodename, macaddress, asleep)
{
    var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("class","badge bg-light-blue-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    cellIndex++;

    // status
    row.appendChild(document.createElement('td'));
    var span_status = document.createElement("span");

    var status = "已離開";
    if ( asleep === "1" )
    {
        status = "休眠中";
        span_status.setAttribute("class","badge bg-yellow");
    }
    else
    {
        span_status.setAttribute("class","badge bg-fuchsia");
    }
    span_status.textContent = status;
    row.cells[cellIndex].appendChild(span_status);
    cellIndex++;

    row.appendChild(document.createElement('td'));
    var div_form_group = document.createElement("div");
    div_form_group.setAttribute("class","form-group");
    row.cells[cellIndex].appendChild(div_form_group);

    // radio_1
    var div_radio_1 = document.createElement("div");
    div_radio_1.setAttribute("class","radio");
    div_form_group.appendChild(div_radio_1);

    var div_radio_1_label = document.createElement("label");
    div_radio_1.appendChild(div_radio_1_label);

    var div_radio_1_label_input = document.createElement("input");
    div_radio_1_label_input.setAttribute("type","radio");
    div_radio_1_label_input.setAttribute("name","optionRadio_"+macaddress);
    //div_radio_1_label_input.setAttribute("id","optionRadio1_"+macaddress);
    div_radio_1_label_input.setAttribute("value","option1");
    div_radio_1_label_input.setAttribute("checked","true");
    //div_radio_1_label_input.setAttribute("onclick","optionRadio_onclick(1, \""+macaddress+"\");");
    div_radio_1_label.appendChild(div_radio_1_label_input);
    div_radio_1_label.appendChild(document.createTextNode("維持"));


    // radio_2
    var div_radio_2 = document.createElement("div");
    div_radio_2.setAttribute("class","radio");
    div_form_group.appendChild(div_radio_2);

    var div_radio_2_label = document.createElement("label");
    div_radio_2.appendChild(div_radio_2_label);

    var div_radio_2_label_input = document.createElement("input");
    div_radio_2_label_input.setAttribute("type","radio");
    div_radio_2_label_input.setAttribute("name","optionRadio_"+macaddress);
    //div_radio_2_label_input.setAttribute("id","optionRadio2_"+macaddress);
    div_radio_2_label_input.setAttribute("value","option2");
    //div_radio_2_label_input.setAttribute("onclick","optionRadio_onclick(2, \""+macaddress+"\");");
    div_radio_2_label.appendChild(div_radio_2_label_input);
    div_radio_2_label.appendChild(document.createTextNode("移動(搬到暫存區域)"));

    cellIndex++;

    return row;
}

function fixdevice_update()
{
    document.getElementById("fixdevice_updatebtn").disabled = true;
    document.getElementById("fixdevice_sending_callout").style.display = 'block';
    document.getElementById("fixdevice_failed_callout").style.display = 'none';
    document.getElementById("fixdevice_finished_callout").style.display = 'none';

    var updateNode = [];

    for (var i = 0; i < glNonAliveTag_List.length; ++i)
    {
        var nodeData = glNonAliveTag_List[i];
        var macaddress = nodeData[6];

        var radios = document.getElementsByName("optionRadio_"+macaddress);
        if (radios[0].checked)
        {
            console.log(macaddress + ":" + radios[0].value);
        }
        else
        if (radios[1].checked)
        {
            console.log(macaddress + ":" + radios[1].value);
            updateNode.push(macaddress);
        }
    }

    if (updateNode.length == 0)
    {
        document.getElementById("fixdevice_sending_callout").style.display = 'none';
        document.getElementById("fixdevice_failed_callout").style.display = 'none';
        document.getElementById("fixdevice_finished_callout").style.display = 'none';

        document.getElementById("fixdevice_reponse_msg_callout").style.display = 'block';
        document.getElementById("fixdevice_reponse_msg_callout").innerHTML = "<p>沒有選擇裝置更新!</p>"

        setTimeout(close_callout, 5000, "fixdevice_reponse_msg_callout");
        document.getElementById("fixdevice_updatebtn").disabled = false;
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['fixdevice'] = '1';
    jsondata['updatenode'] = updateNode;

    jQuery.ajax({
        url: targetURL,
        type: 'POST',
        data: jsondata,
        success:
            function(response)
            {
                document.getElementById("fixdevice_sending_callout").style.display = 'none';
                document.getElementById("fixdevice_failed_callout").style.display = 'none';
                document.getElementById("fixdevice_finished_callout").style.display = 'block';

                setTimeout(close_callout, 5000, "fixdevice_finished_callout");
                document.getElementById("fixdevice_updatebtn").disabled = false;
            },
        error:
            function(xhr, status, error)
            {
                //alert("error = " + error);
                //alert("xhr.responseText = " + xhr.responseText);
                document.getElementById("fixdevice_sending_callout").style.display = 'none';
                document.getElementById("fixdevice_failed_callout").style.display = 'block';
                document.getElementById("fixdevice_finished_callout").style.display = 'none';

                setTimeout(close_callout, 5000, "fixdevice_failed_callout");
                document.getElementById("fixdevice_updatebtn").disabled = false;
            }
    });
}

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}
