/*
 * Author: ikki Chung
 * Date: 2019.12.23
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_DEVICE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var FirstLoadCount = 0;

var glForaData;

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

                load_bodyinfo();

            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_bodyinfo_records();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

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
    }

    if ( CurrentLoadIndex != 4 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_bodyinfo()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadforacaredata=1', function(data)
    {
        glForaData = data;
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;

        show_bodyinfo_records();

        if (FirstLoadCount < 10)
        {
            setTimeout(load_bodyinfo, 5000);
            FirstLoadCount++;
        }
        else
            setTimeout(load_bodyinfo, 60000);
    })
    .error(function() {

    })
    .complete(function() {
    });
}

function show_bodyinfo_records()
{
    clear_table('NODE_BODYINFO_TABLE_HEAD');
    clear_table('NODE_BODYINFO_TABLE');

    var headerData = glForaData.header.split(",");

    create_header(headerData);

    // console.log("header:" + glForaData.header);
    $.each(glForaData.data, function(itemindex, nodeinfo)
    {
        // console.log(itemindex+ " data:" + nodeinfo);

        var bodyinfo = nodeinfo.split(",");
        create_row_data(bodyinfo);

    });
}

function create_header(headerData)
{
    var table_header = document.getElementById('NODE_BODYINFO_TABLE_HEAD');
    var row = document.createElement('tr');
    table_header.appendChild(row);

    for(var i=0;i<headerData.length;i++)
    {
        row.appendChild(document.createElement('th'));
        row.cells[i].appendChild(document.createTextNode(headerData[i]));
    }
}

function create_row_data(bodyinfo)
{
    var table_header = document.getElementById('NODE_BODYINFO_TABLE');
    var row = document.createElement('tr');
    table_header.appendChild(row);

    for(var i=0;i<bodyinfo.length;i++)
    {
        row.appendChild(document.createElement('th'));

        var dataLabel = document.createElement("span");
        dataLabel.setAttribute("class","badge bg-green");

        if (bodyinfo[i] === '&nbsp;')
            dataLabel.textContent = '';
        else
            dataLabel.textContent = bodyinfo[i];
        row.cells[i].appendChild(dataLabel);
    }
}

function clear_table(tableid)
{
    var bodyinfo_table = document.getElementById(tableid);

    while (bodyinfo_table.firstChild)
    {
        bodyinfo_table.removeChild(bodyinfo_table.firstChild);
    }
}

function open_bodyinfoforacare_page()
{
    var win = window.open('/index.php/vilsbodyinfoforacare/'+project_id, '_blank');
    win.focus();
}
