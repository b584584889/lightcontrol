/*
 * Author: ikki Chung
 * Date: 2019 06 12
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds
var RECORD_PAGE_COUNT = 20;

var gl_tag_num = 0;
var gl_tag_load_index = 0;

var glTag_List = [];

var glTargetStartDate = null;
var glTargetEndDate = null;
var glCurrentTag_nodeid = "";
var glCurrentTag_clicktime = new Date('2019/01/01');
var glCurrentTag_Records = [];
var glCurrentRecord_page = 0;

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
                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                create_record_table();
                click_tag("");
                CurrentLoadIndex = 2; // next state
            }
        break;
        case 2:
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

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);

        gl_tag_load_index = 0;
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

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

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
            add_tag_to_table();
            CurrentLoadIndex = 1; // next state
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

function add_tag_to_table()
{
    var tag_list_content = document.getElementById('tag_list_content');

    while (tag_list_content.rows.length >= 1)
    {
        tag_list_content.deleteRow(0);
    }

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        tag_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }

}

function click_tag(click_node_nodeid)
{
    //console.log("click_tag() click_node_nodeid:" + click_node_nodeid);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    var pdfreport_status = document.getElementById("pdfreport_status");
    while (pdfreport_status.firstChild)
    {
        pdfreport_status.removeChild(pdfreport_status.firstChild);
    }

    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentTag_nodeid = node_nodeid;
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    glCurrentRecord_page = 0;
    load_tag_record();
}

function create_record_table()
{
    var record_box_content = document.getElementById('record_box_content');

    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table");
    record_box_content.appendChild(divTabPane);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

    //
    // box header
    //
    var boxHeader = document.createElement('div');
    boxHeader.setAttribute("class","box-header");
    boxGroup.appendChild(boxHeader);

    // box-title
    var boxTitle = document.createElement('H3');
    boxTitle.setAttribute("class","box-title");
    boxHeader.appendChild(boxTitle);

    var spanTitle = document.createElement("span");
    spanTitle.setAttribute("class","badge bg-green");
    spanTitle.setAttribute("id","record_box_table_title");
    boxTitle.appendChild(spanTitle);

    //===============================
    var footer_btn_2 = document.createElement("a");
    footer_btn_2.setAttribute("class","btn btn-success btn-xs");
    footer_btn_2.setAttribute("onclick","create_tag_pdf_report();");
    footer_btn_2.setAttribute("id","pdfreport_btn");
    footer_btn_2.textContent = "產生PDF報告";
    boxTitle.appendChild(footer_btn_2);

    var footer_btn_2_spen = document.createElement("spen");
    footer_btn_2_spen.setAttribute("id","pdfreport_status");
    footer_btn_2_spen.textContent = "";
    boxTitle.appendChild(footer_btn_2_spen);
    //===============================

    // space
    var spenspace = document.createElement('spen');
    spenspace.textContent = "  ";
    boxTitle.appendChild(spenspace);

    //===============================
    var footer_btn_xlsx = document.createElement("a");
    footer_btn_xlsx.setAttribute("class","btn btn-success btn-xs");
    footer_btn_xlsx.setAttribute("onclick","create_tag_xlsx_report();");
    footer_btn_xlsx.setAttribute("id","xlsxreport_btn");
    footer_btn_xlsx.textContent = "產生XLSX報告";
    boxTitle.appendChild(footer_btn_xlsx);

    var footer_btn_xlsx_spen = document.createElement("spen");
    footer_btn_xlsx_spen.setAttribute("id","xlsxreport_status");
    footer_btn_xlsx_spen.textContent = "";
    boxTitle.appendChild(footer_btn_xlsx_spen);
    //===============================

    var spanTitleStartTime = document.createElement("span");
    spanTitleStartTime.setAttribute("class","badge bg-yellow-active");
    spanTitleStartTime.setAttribute("id","record_StartEndTime");
    boxTitle.appendChild(spanTitleStartTime);

    // box-tools
    var divTools = document.createElement("div");
    divTools.setAttribute("class","box-tools");
    boxHeader.appendChild(divTools);

    var ulTools = document.createElement("ul");
    ulTools.setAttribute("class","pagination pagination-sm no-margin pull-right");
    divTools.appendChild(ulTools);

    // '<'
    var liTools1 = document.createElement("li");
    ulTools.appendChild(liTools1);

    var aTools1 = document.createElement("a");
    aTools1.setAttribute("href","javascript:;");
    aTools1.setAttribute("onclick","move_date_forward();return false;");
    aTools1.textContent = '\xAB'; // html &laquo;
    liTools1.appendChild(aTools1);

    // 'Page'
    var liTools2 = document.createElement("li");
    ulTools.appendChild(liTools2);

    var aTools2 = document.createElement("a");
    aTools2.setAttribute("href","#");
    aTools2.setAttribute("id","record_Status_Page");
    aTools2.textContent = "";
    liTools2.appendChild(aTools2);

    // '>'
    var liTools3 = document.createElement("li");
    ulTools.appendChild(liTools3);

    var aTools3 = document.createElement("a");
    aTools3.setAttribute("href","javascript:;");
    aTools3.setAttribute("onclick","move_date_backward();return false;");
    aTools3.textContent = '\xBB'; //html &raquo;
    liTools3.appendChild(aTools3);

    //
    // box-body
    //
    var boxBody = document.createElement("div");
    boxBody.setAttribute("class","box-body no-padding");
    boxGroup.appendChild(boxBody);

    // table
    var tableGroup = document.createElement('table');
    tableGroup.setAttribute("class","table table-bordered");
    boxBody.appendChild(tableGroup);

    var theadGroup = document.createElement("thead");
    tableGroup.appendChild(theadGroup);

    var theadtrGroup = document.createElement("tr");
    theadtrGroup.setAttribute("bgcolor","#cce6ff");
    theadtrGroup.setAttribute("id","record_table_head");
    theadGroup.appendChild(theadtrGroup);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtrGroup.appendChild(theadth);

    var theadthAreaName = document.createElement("th");
    theadthAreaName.setAttribute("style","width: 150px");
    theadthAreaName.textContent = '區域名稱';
    theadtrGroup.appendChild(theadthAreaName);

    var theadth_EnterTime = document.createElement("th");
    theadth_EnterTime.setAttribute("style","width: 200px; text-align:center");
    theadth_EnterTime.textContent = '進入時間';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","width: 200px; text-align:center");
    theadth_LeaveTime.textContent = '離開時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var theadth_StayTime = document.createElement("th");
    //theadth_StayTime.setAttribute("style","width: 50px; text-align:center");
    theadth_StayTime.textContent = '停留時間';
    theadtrGroup.appendChild(theadth_StayTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_body");
    tableGroup.appendChild(tbodyGroup);

    if (glTargetStartDate == undefined)
    {
        glTargetStartDate = getTargetDate(-7);
    }
    if (glTargetEndDate == undefined)
    {
        glTargetEndDate = getTargetDate(0);
    }

    //Date picker
    $('#record_Status_Page').daterangepicker({
        autoApply: true,
        manualInputs: false, //SMIMS ikki add
        startDate: glTargetStartDate,
        endDate: glTargetEndDate,
        locale: {
                format: 'YYYY/MM/DD'
            }

    },
    function(start, end)
    {
        // console.log("A new date selection was made: " + start.format('YYYY-MM-DD') + ' to ' + end.format('YYYY-MM-DD') );

        glTargetStartDate = start.format('YYYYMMDD');
        glTargetEndDate = end.format('YYYYMMDD');

        load_tag_record();

    });

}

function move_date_forward()
{
    var end_date = glTargetEndDate;
    var cur_date = moment(end_date, "YYYYMMDD");
    var new_end_date = cur_date.add(7, 'day').format('YYYYMMDD');
    var new_start_date = end_date;

    glTargetStartDate = new_start_date;
    glTargetEndDate = new_end_date;

    console.log("move_date_forward()");

    glCurrentRecord_page = 0;
    load_tag_record();
}

function move_date_backward()
{
    var start_date = glTargetStartDate;
    var cur_date = moment(start_date, "YYYYMMDD");
    var new_start_date = cur_date.subtract(7, 'day').format('YYYYMMDD');
    var new_end_date = start_date;

    glTargetStartDate = new_start_date;
    glTargetEndDate = new_end_date;

    console.log("move_date_backward()");

    glCurrentRecord_page = 0;
    load_tag_record();
}

function load_tag_record()
{
    console.log("load_tag_record() glCurrentTag_nodeid:" + glCurrentTag_nodeid);

    var chrTagName = "";
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( glCurrentTag_nodeid === node_nodeid )
        {
            chrTagName = node_name;
        }
    }

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = chrTagName;
    }
    var record_Status_Page = document.getElementById("record_Status_Page");
    if (record_Status_Page != undefined)
    {
        record_Status_Page.textContent = convertToShowDate(glTargetStartDate) +" - "+ convertToShowDate(glTargetEndDate);
    }
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body != undefined)
    {
        while (record_table_body.rows.length >= 1)
        {
            record_table_body.deleteRow(0);
        }
    }
    var record_box_move_page_index = document.getElementById('record_box_move_page_index');
    if (record_box_move_page_index != undefined)
    {
        record_box_move_page_index.textContent = glCurrentRecord_page + 1;
    }

    var targetStartDate = getTargetDateTime(glTargetStartDate, 0);
    var targetEndDate = getTargetDateTime(glTargetEndDate, 1);
    var offset = glCurrentRecord_page * RECORD_PAGE_COUNT;
    var count = RECORD_PAGE_COUNT;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadareainout=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {

        glCurrentTag_Records = [];
        //glCurrentRecord_page = 0;

        $.each(data, function(key, value)
        {
            var record = {};

            record.AreaType = value['AreaType'];
            record.action = value['action']; //1:in 2:out
            record.AreaID = value['AreaID'];
            record.AreaName = value['AreaName'];
            record.DataTime = value['DataTime'];

            // check if same time, let 2:out in the front
            if ( glCurrentTag_Records.length > 0 )
            {
                var pre_record = glCurrentTag_Records[ glCurrentTag_Records.length - 1];

                if ( pre_record.DataTime === record.DataTime )
                {
                    if ( pre_record.action === '2' && record.action === '1')
                    {
                        // switch

                        console.log("load_tag_record() switch pre_record.AreaName:" + pre_record.AreaName + ", record.AreaName:"+record.AreaName);

                        glCurrentTag_Records[ glCurrentTag_Records.length - 1] = record;
                        record = pre_record;
                    }
                    else
                    {
                        // do nothing
                    }
                }
            }

            glCurrentTag_Records.push(record);



            // console.log("load_tag_record() key:"+key+", AreaType:"+AreaType+", action:"+action+", AreaName:"+AreaName+", DataTime:"+DataTime);
        });

        console.log("load_tag_record() glCurrentTag_Records.length:"+glCurrentTag_Records.length);

        put_record_to_table(glCurrentTag_nodeid, glCurrentTag_Records, glCurrentRecord_page);

    }).success(function(){})
    .error(function(){})
    .complete(function(){});

}

function put_record_to_table(nodeid, records, page)
{
    var pre_record = null;

    var ui_index = 0;
    for(index in records)
    {
        var record = records[index];
        // console.log("load_tag_record() AreaType:"+record.AreaType+", action:"+record.action+", AreaName:"+record.AreaName+", DataTime:"+record.DataTime);

        if ( pre_record == null )
        {
            pre_record = record;
        }
        else
        {
            if ( pre_record.AreaID === record.AreaID )
            {
                if ( pre_record.DataTime === record.DataTime )
                {
                    create_area_inout_row(ui_index, pre_record, record);
                    ui_index++;

                    pre_record = null; // clear record
                }
                else
                if ( pre_record.action === '2' && record.action === '1' )
                {
                    create_area_inout_row(ui_index, pre_record, record);
                    ui_index++;

                    pre_record = null; // clear record
                }
                else
                {
                    // creat a row for single event, only in or out
                    create_area_row(ui_index, pre_record);
                    ui_index++;

                    pre_record = record;
                }
            }
            else
            {
                // creat a row for single event, only in or out

                create_area_row(ui_index, pre_record);
                ui_index++;

                pre_record = record;
            }
        }
    }

    if ( pre_record != null )
    {
        create_area_row(ui_index, pre_record);
        ui_index++;
    }

    if (ui_index == 0)
    {
        var nodata_record = {};
        nodata_record.AreaType = 0;
        nodata_record.action = 0; //1:in 2:out
        nodata_record.AreaID = 0;
        nodata_record.AreaName = '無資料';
        nodata_record.DataTime = "";
        create_area_inout_row(0, nodata_record, nodata_record);
        return;
    }

}

function create_area_inout_row(ui_index, pre_record, record)
{
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body == undefined)
    {
        console.log("create_area_inout_row() record_table_body == undefined");
        return ;
    }

    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);
    record_table_body.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(pre_record.AreaName));
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    var table_td_entertime_span = document.createElement('span');
    table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
    table_td_entertime_span.textContent = record.DataTime;
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.textContent = pre_record.DataTime;
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    var stayTime = moment.utc(moment(pre_record.DataTime,"YYYY-MM-DD HH:mm:ss").diff(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(stayTime));
    cellIndex++;

}

function create_area_row(ui_index, record)
{
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body == undefined)
    {
        console.log("create_area_inout_row() record_table_body == undefined");
        return ;
    }

    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);
    record_table_body.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(record.AreaName));
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    if (record.action == '1')
    {
        var table_td_entertime_span = document.createElement('span');
        table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
        table_td_entertime_span.textContent = record.DataTime;
        table_td_entertime_td.appendChild(table_td_entertime_span);
    }
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    if (record.action == '2')
    {
        var table_td_leaveltime_span = document.createElement('span');
        table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
        table_td_leaveltime_span.textContent = record.DataTime;
        table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    }
    cellIndex++;

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;
}

function record_box_move_page_forward()
{
    if ( glCurrentRecord_page <= 0 )
    {
        console.log("record_box_move_page_forward() no more data!!");
        // not more data
        return;
    }
    else
    {
        glCurrentRecord_page--;
        load_tag_record();
    }
}

function record_box_move_page_backward()
{
    if ( glCurrentTag_Records.length < RECORD_PAGE_COUNT)
    {
        console.log("record_box_move_page_backward() no more data!!");
        // not more data
        return;
    }
    else
    {
        glCurrentRecord_page++;
        load_tag_record();
    }
}

function create_tag_pdf_report()
{
    var pdfreport_status = document.getElementById("pdfreport_status");
    pdfreport_status.textContent = "產生報表中...";

    var targetStartDate = convertToQueryDate(glTargetStartDate);
    var targetEndDate = convertToQueryDate(glTargetEndDate);

    var SendDate = "loadareainoutpdfreport=1&id="+glCurrentTag_nodeid+"&project_id="+project_id+"&startdate=" + targetStartDate+"&enddate=" + targetEndDate;

    var targetURL = gl_target_server + "/php/configvils.php";

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
            console.log("create_tag_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdfreport_status = document.getElementById("pdfreport_status");
            while (pdfreport_status.firstChild)
            {
                pdfreport_status.removeChild(pdfreport_status.firstChild);
            }

            if ( result.success === 'true')
            {
                var daily_pdf_report_dl_btn = document.createElement("button");
                daily_pdf_report_dl_btn.setAttribute("type", 'button');
                daily_pdf_report_dl_btn.setAttribute("class", 'btn btn-app');
                //daily_pdf_report_dl_btn.setAttribute("onclick", "location.href='" +result.pdfreport +"'");
                daily_pdf_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.pdfreport +"');");
                pdfreport_status.appendChild(daily_pdf_report_dl_btn);

                var imgPdf = document.createElement("img");
                imgPdf.setAttribute("src", "/images/icons8-export-pdf-96.png");
                imgPdf.setAttribute("alt", "Download report");
                imgPdf.setAttribute("title", "Download report");
                imgPdf.setAttribute("height", "42");
                imgPdf.setAttribute("width", "42");
                daily_pdf_report_dl_btn.appendChild(imgPdf);
            }
            else
            {
                var pdfreport_status = document.getElementById("pdfreport_status");
                pdfreport_status.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function create_tag_xlsx_report()
{
    var xlsxreport_status = document.getElementById("xlsxreport_status");
    xlsxreport_status.textContent = "產生報表中...";

    var targetStartDate = convertToQueryDate(glTargetStartDate);
    var targetEndDate = convertToQueryDate(glTargetEndDate);

    var SendDate = "loadareainoutxlsxreport=1&id="+glCurrentTag_nodeid+"&project_id="+project_id+"&startdate=" + targetStartDate+"&enddate=" + targetEndDate;

    var targetURL = gl_target_server + "/php/configvils.php";

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
            console.log("create_tag_xlsx_report() success:"+result.success+" xlsxreport:"+result.xlsxreport);

            var xlsxreport_status = document.getElementById("xlsxreport_status");
            while (xlsxreport_status.firstChild)
            {
                xlsxreport_status.removeChild(xlsxreport_status.firstChild);
            }

            if ( result.success === 'true')
            {
                var daily_xlsx_report_dl_btn = document.createElement("button");
                daily_xlsx_report_dl_btn.setAttribute("type", 'button');
                daily_xlsx_report_dl_btn.setAttribute("class", 'btn btn-app');
                daily_xlsx_report_dl_btn.setAttribute("style", "width:90px;height:100px");
                //daily_pdf_report_dl_btn.setAttribute("onclick", "location.href='" +result.pdfreport +"'");
                daily_xlsx_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.xlsxreport +"');");
                xlsxreport_status.appendChild(daily_xlsx_report_dl_btn);

                var imgPdf = document.createElement("img");
                imgPdf.setAttribute("src", "/images/xlsx-dl-icon-1.png");
                imgPdf.setAttribute("alt", "Download report");
                imgPdf.setAttribute("title", "Download report");
                imgPdf.setAttribute("height", "80");
                imgPdf.setAttribute("width", "80");
                daily_xlsx_report_dl_btn.appendChild(imgPdf);
            }
            else
            {
                var xlsxreport_status = document.getElementById("xlsxreport_status");
                xlsxreport_status.textContent = "產生報表失敗!";
            }
        }
    });
    return false;

}

// get today date
function getTodayDate()
{
    var Today = new Date();
    today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();

    //today = "20181218";
    return today;
}

function getTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function getTargetDateTime(inDate, addDays)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    //var fullDatetime = cur_date.format('YYYY-MM-DD HH:mm:ss');

    var targetDate = cur_date.add(addDays, 'd').format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertToQueryDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY-MM-DD');

    return outDate;
}

function convertToShowDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY/MM/DD');

    return outDate;
}

function convertRecordTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD HH:mm:ss");
    var outDate = cur_date.format('HH:mm:ss');

    return outDate;
}

function open_new_page(openurl)
{
    var win = window.open(openurl, '_blank');
    win.focus();
}

function open_footprintinout_page()
{
    var win = window.open('/index.php/vilsfootprintinout/'+project_id, '_blank');
    win.focus();
}
