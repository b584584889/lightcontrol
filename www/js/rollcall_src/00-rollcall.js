/*
 * Author: ikki Chung
 * Date: 2017.12.06
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


//
// globa value
//
var gl_all_tag = [];
var gl_area_tag = [];
var gl_none_tag = [];
var gl_tag_num = 0;
var gl_tag_load_index = 0;
var LOAD_DEVICE_NUM = 20;

//
// start load location
//
load_Tags_Info();

function load_Tags_Info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_tag_load_index = 0;
    })
    .success(function() {
        setTimeout(load_Tags, 10);
    })
    .error(function() {
        setTimeout(load_Tags_Info, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_Tags()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        return;
    }
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtagrollcall=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&areaid='+rollcall_areaid+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            // split data
            var node = data[key];
            var nodeData = node.split(",");
            var alive = nodeData[1];
            var areaid = nodeData[6];
            var areaname = nodeData[7];
            var AreaAction = nodeData[8]; // 0:normal 1:in  2:out
            var asleep = nodeData[10];

            console.log("load_Tags() nodeid:"+nodeData[3] +" areaid:"+areaid +" rollcall_areaid:"+rollcall_areaid +", alive:"+alive);

            gl_all_tag.push(nodeData);

            if ( (areaid == rollcall_areaid) && (AreaAction == '1'))
            {
                if ( (alive == '0') && (asleep == '0') )
                    gl_none_tag.push(nodeData);
                else
                    gl_area_tag.push(nodeData);
            }
            else
                gl_none_tag.push(nodeData);
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            draw_table();

            //setTimeout(load_Tags_Info, 500); // update display every 500 ms
        }
        else
        {
            setTimeout(load_Tags, 10);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_Tags, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function draw_table()
{
    //console.log("draw_table() gl_all_tag.length:"+gl_all_tag.length);

    var rollcall_sb_text = document.getElementById('rollcall_sb');
    var rollcall_rb_text = document.getElementById('rollcall_rb');
    var rollcall_nb_text = document.getElementById('rollcall_nb');
    var rollcall_time_text = document.getElementById('rollcall_time');

    rollcall_sb_text.textContent = gl_all_tag.length;
    rollcall_rb_text.textContent = gl_area_tag.length;
    rollcall_nb_text.textContent = gl_none_tag.length;
    rollcall_time_text.textContent = new Date().toLocaleString();


    var loc_table = document.getElementById('NODE_TABLE_BODY');

    while (loc_table.rows.length >= 1)
         loc_table.deleteRow(0);

     var row = create_rollcall_table_item("實到", 1, gl_area_tag);
     loc_table.appendChild(row);

     var row = create_rollcall_table_item("未到", 2, gl_none_tag);
     loc_table.appendChild(row);
}

function create_rollcall_table_item(show_status, type, nodelist)
{
    var row = document.createElement('tr');
    var cellIndex = 0;

    // status
    var tdName = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdName);
    var Spanmac = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    Spanmac.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    Spanmac.textContent = show_status;
    row.cells[cellIndex].appendChild(Spanmac);
    cellIndex++;

    // Node Count
    row.appendChild(document.createElement('td'));
/*
    var Spanmac = document.createElement("span");
    if (nodelist.length == 0)
        Spanmac.setAttribute("class","badge bg-red");
    else
        Spanmac.setAttribute("class","badge bg-blue");
    Spanmac.textContent = nodelist.length;
    row.cells[cellIndex].appendChild(Spanmac);
*/
    //========
    var DivColorPaletteSet = document.createElement("div");
    DivColorPaletteSet.setAttribute("class","color-palette-set");
    row.cells[cellIndex].appendChild(DivColorPaletteSet);

    // color-palette Orange
    var DivColorPalette1 = document.createElement("div");
    //DivColorPalette1.setAttribute("class","bg-orange disabled color-palette");
    DivColorPalette1.setAttribute("style","text-align:center;");
    DivColorPaletteSet.appendChild(DivColorPalette1);

    var Span1 = document.createElement("span");
    Span1.textContent = nodelist.length;
    Span1.setAttribute("style","font-size:18px; font-weight:bold; color:#003300;");
    DivColorPalette1.appendChild(Span1);

    //========
    cellIndex++;

    // node id / name
    row.appendChild(document.createElement('td'));

    for (var i = 0; i < nodelist.length; ++i)
    {
        var nodeData = nodelist[i];

        var nodeid = nodeData[3];
        var nodename = decodeURIComponent(nodeData[4]);

        var connected = true;
        if (nodeData[0] == '-1')
            connected = false;

        var alive = nodeData[1];
        var action = nodeData[8];
        var actiontime = nodeData[9];

        var BlockColor = "bg-green";
        if (type == 1)
        {
            BlockColor = "bg-green";
        }
        if (type == 2)
        {
            BlockColor = "bg-red";
        }

        var DivCol = document.createElement("div");
        DivCol.setAttribute("class","col-sm-4 col-md-2");
        row.cells[cellIndex].appendChild(DivCol);

        var DivColorPaletteSet = document.createElement("div");
        DivColorPaletteSet.setAttribute("class","color-palette-set");
        DivCol.appendChild(DivColorPaletteSet);

        //========
        // color-palette 1
        var DivColorPalette1 = document.createElement("div");
        DivColorPalette1.setAttribute("class",BlockColor + " disabled color-palette");
        DivColorPalette1.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette1);

        var Span1 = document.createElement("span");
        Span1.textContent = nodeid;
        Span1.setAttribute("style","font-size:18px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette1.appendChild(Span1);
        //========

        //========
        // color-palette 2
        var DivColorPalette2 = document.createElement("div");
        DivColorPalette2.setAttribute("class",BlockColor + " color-palette");
        DivColorPalette2.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette2);

        var Span2 = document.createElement("span");
        Span2.textContent = nodename;
        Span2.setAttribute("style","font-size:16px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette2.appendChild(Span2);
        //========

        //========
        // color-palette 3
        var DivColorPalette3 = document.createElement("div");
        DivColorPalette3.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette3.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette3);

        var Span3 = document.createElement("span");
        Span3.textContent = "";
        //Span3.setAttribute("style","font-size:14px; font-weight:bold; color:#020202;");

        var Span3_href = document.createElement("a");
        Span3_href.setAttribute("href","javascript:;");
        Span3_href.setAttribute("onclick","show_on_2dmap(\""+nodeid+"\");return false;");
        Span3_href.setAttribute("style","font-size:14px; color:#222222;");
        Span3_href.title = '跳轉到2D地圖';
        Span3_href.textContent = '查看地圖';
        Span3.appendChild(Span3_href);

        DivColorPalette3.appendChild(Span3);
        //========

        //========
        // color-palette 4
        var DivColorPalette4 = document.createElement("div");
        //DivColorPalette4.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette4.setAttribute("style","text-align:center; color:#e6f5ff;");
        DivColorPaletteSet.appendChild(DivColorPalette4);

        var Span4 = document.createElement("span");
        Span4.textContent = "-";
        Span4.setAttribute("style","font-size:14px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette4.appendChild(Span4);
        //========

    }

    cellIndex++;

    return row;
}

function create_tag_pdf_report()
{
    var pdfreport_status = document.getElementById("pdfreport_status");
    pdfreport_status.textContent = "產生報表中...";

    var SendDate = 'loadtagrollcallpdfreport=1&areaid='+rollcall_areaid+'&project_id='+project_id;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

    var SendDate = 'loadtagrollcallxlsxreport=1&areaid='+rollcall_areaid+'&project_id='+project_id;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

function show_on_2dmap(nodeid)
{
    window.open("/index.php/vils2dmap/"+project_id+"?nodeid="+nodeid,"_self")
}

function open_new_page(openurl)
{
    var win = window.open(openurl, '_blank');
    win.focus();
}

function open_location_page()
{
    var win = window.open('/index.php/vilslocation/'+project_id, '_blank');
    win.focus();
}
