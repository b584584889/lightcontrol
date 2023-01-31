/*
 * Author: ikki Chung
 * Date: 26 Jul 2018
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var glFileList = [];
var glFileLoad = [];

//
// start run
//
var glLiveMap = new Indoor2DMap();

load_map_config();
load_log_file_list();

//INITIALIZE SPARKLINE CHARTS
$(".sparkline").each(function () {
  var $this = $(this);
  $this.sparkline('html', $this.data());
});

function load_map_config()
{
    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
    {
        var data = mapsdata[0];
        
        //
        // get buildind id and floor
        //
        var targetURL = gl_target_server + "/php/configvils.php";
        $.getJSON(targetURL, 'loadprojectconfig=1'+'&project_id='+project_id , function(configdata)
        {
            var BuildingID = configdata.BuildingID;
            var BuildingFloor = configdata.BuildingFloor;

            glLiveMap.setMapCoeff(data.POSX, data.POSY, data.MAPWIDTH, data.MAPHEIGHT, data.SCALE, data.COEFF);
            glLiveMap.setMapImage(data.FILE, "Indoor 2D Map");
            glLiveMap.setBuildingInfo(BuildingID, BuildingFloor);
            //glLiveMap.createMapEditor();
            //glLiveMap.load_maparea();
            //glLiveMap.setNodeDragCallback(update_map_node_info);
            //glLiveMap.setOrigNodeDragCallback(update_orig_node_info);
            //glLiveMap.setUpdateMapInfoCallback(update_map_info);
            //glLiveMap.setNodeClickCallback(node_click);
            glLiveMap.addOriginMarker(0, 0, 0);

            load_anchors();

        });
    });
    return false;
}

var glAnchor_List = [];
function load_anchors()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";

    $.getJSON(targetURL, 'loadanchor=1'+'&project_id='+project_id, function(data)
    {
        var loc_anchor_num = 0;
        if (data['LOC_ANCHOR_NUM'] != undefined && data['LOC_ANCHOR_NUM'] != null)
            loc_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        if (loc_anchor_num > 0)
            glAnchor_List = [];


        for (var i = 0; i < loc_anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var nodeData = data[key].split(",");
            var macaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var buildingid = nodeData[13];
            var floor = nodeData[14];
            var date = new moment();
            //macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date
            glLiveMap.updateAnchorMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date);

            glAnchor_List.push(nodeData);
        }
    });
}

function load_log_file_list()
{
    $.ajax({
        url : "/log",
        success : function(result)
        {
            //alert(result);
            //console.log(result);
            //console.log(HTMLtoXML(result));

            var json = html2json(result);
            //console.log(json);
            //console.log(json.child[0]);
            //console.log(json.child[0].child[3]);
            //console.log(json.child[0].child[3].child[3]);
            //console.log(json.child[0].child[3].child[3].child.length);



            var parent = json.child[0].child[3].child[3].child;
            for(child in parent)
            {
                //console.log(parent[child]);
                if (parent[child].child != undefined)
                {
                    if (parent[child].child.length >= 5)
                    {
                        var curChild = parent[child].child;
                        //console.log(curChild);
                        for( cidx in curChild)
                        {
                            if (curChild[cidx].tag != 'td')
                            {
                                continue;
                            }
                            //console.log(curChild[cidx]);
                            if (curChild[cidx].child != undefined)
                            {
                                for( tdidx in curChild[cidx].child)
                                {
                                    if (curChild[cidx].child[tdidx].tag != 'a')
                                    {
                                        continue;
                                    }
                                    var curattr = curChild[cidx].child[tdidx].attr;
                                    //console.log(curattr.href);

                                    if (curattr.href.length > 2)
                                    {
                                        var anchorIdx = curattr.href.indexOf("anchors");
                                        var csvIdx = curattr.href.indexOf(".csv");
                                        if ( anchorIdx < 0 && csvIdx > 0)
                                        {
                                            glFileList.push(curattr.href);
                                            glFileLoad.push(false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //
            // all files list
            //
            //console.log(glFileList);
            ceate_dropdown_filelist();
        }
    });


    // url_content("/log")
    // .success(function(data)
    // {
    //     //alert(data);
    //     //var html = document.body.innerHTML.trim();
    //     console.log(data);
    //     //console.log(HTMLtoXML(data));
    // });

    // var json = html2json(html);
    // console.log(JSON.stringify(json, ' ', ' '));
    // console.assert(html === json2html(json));
}


function ceate_dropdown_filelist()
{
    var dropdown_files_ul = document.getElementById("dropdown_files");

    for (var fileidx in glFileList)
    {
        var file = glFileList[fileidx];
        var item_li = create_dropdown_item(fileidx, file);
        dropdown_files_ul.appendChild(item_li);

        //document.getElementById("dropdown_checkbox_"+fileidx).checked = group.visible;
    }
}

function create_dropdown_taglist()
{
    var tagList = glLiveMap.getTagList();
    //console.log("tagList:"+tagList);
    var dropdown_tags_ul = document.getElementById("dropdown_tags");
    while (dropdown_tags_ul.hasChildNodes())
    {
        dropdown_tags_ul.removeChild(dropdown_tags_ul.firstChild);
    }

    var index;
    for	(index = 0; index < tagList.length; index++)
    {
        exist_node = tagList[index];
        //console.log("Tag:"+exist_node);
        var item_li = create_dropdown_tag_item(index, exist_node);
        dropdown_tags_ul.appendChild(item_li);
    }
}

function create_dropdown_item(fileindex, showtext)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", showtext:"+showtext);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", fileindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_item_click(\""+ fileindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_"+fileindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_click(\""+ fileindex +"\");return true;");

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+showtext));
    item_li.appendChild(item_click);

    return item_li;
}

function create_dropdown_tag_item(tagindex, tagname)
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
    item_checkbox.checked = true;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+tagname));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_click = false;
function dropdown_item_click(fileindex)
{
    var index;
    for	(index = 0; index < glFileLoad.length; index++)
    {
        if ( index == fileindex)
        {
            glFileLoad[index] = true;
            document.getElementById("dropdown_checkbox_"+index).checked = true;
        }
        else
        {
            glFileLoad[index] = false;
            document.getElementById("dropdown_checkbox_"+index).checked = false;
        }
    }
    glLiveMap.clearAll();
    load_csv_files(true);

    glDropdown_checkbox_click = false;
}

//
// show multiple files
//
// function dropdown_item_click(fileindex)
// {
//     //console.log("dropdown_item_click() nodeid:"+nodeid);
//     var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_"+fileindex).checked;
//
//     if (glDropdown_checkbox_click)
//     {
//         // keep the same value
//     }
//     else
//     {
//         // toggle value
//         document.getElementById("dropdown_checkbox_"+fileindex).checked = !checkedValue_dropdown_checkbox;
//         checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_"+fileindex).checked;
//     }
//
//     //
//     // update to map
//     //
//     glFileLoad[fileindex] = checkedValue_dropdown_checkbox;
//     load_csv_files(true);
//     //glLiveMap.showTag(nodeid, checkedValue_dropdown_checkbox);//show tag
//
//     glDropdown_checkbox_click = false;
// }


var glDropdown_checkbox_tag_click = false;
function dropdown_tag_click(tagindex)
{
    //console.log("dropdown_item_click() nodeid:"+nodeid);
    var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_tag_"+tagindex).checked;

    if (glDropdown_checkbox_tag_click)
    {
        // keep the same value
    }
    else
    {
        // toggle value
        document.getElementById("dropdown_checkbox_tag_"+tagindex).checked = !checkedValue_dropdown_checkbox;
        checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_tag_"+tagindex).checked;
    }

    //console.log("tagindex:"+tagindex +", show:"+checkedValue_dropdown_checkbox);
    glLiveMap.showTagTrack(tagindex, checkedValue_dropdown_checkbox);

    glDropdown_checkbox_tag_click = false;
}

var glLoafCSVFileIndex = -1;
function load_csv_files(statrLoad)
{
    if (statrLoad)
    {
        glLoafCSVFileIndex = 0;
    }

    //console.log('load_csv_files() statrLoad:' + statrLoad + ' glFileLoad.length:' + glFileLoad.length +' glLoafCSVFileIndex:' + glLoafCSVFileIndex);

    if (glLoafCSVFileIndex < glFileLoad.length)
    {
        if ( glFileLoad[glLoafCSVFileIndex] )
        {
            console.log('load:' + glFileList[glLoafCSVFileIndex]);

            $.ajax({
                url : "/log/" + glFileList[glLoafCSVFileIndex],
                success : function(result)
                {
                    var traceCSVArray = $.csv.toArrays(result);
                    glLiveMap.updateTracker(glFileList[glLoafCSVFileIndex], traceCSVArray);

                    //
                    // Standard Deviation
                    //
                    //calculate_standard_deviation(glFileList[glLoafCSVFileIndex], traceCSVArray);

                    glLoafCSVFileIndex++;
                    load_csv_files(false);
                }
            });
        }
        else
        {
            glLoafCSVFileIndex++;
            load_csv_files(false);
        }
    }
    else
    {
        // create tag dropdown list
        create_dropdown_taglist();
    }
}


function dropdown_checkbox_click(fileindex)
{
    glDropdown_checkbox_click = true;
}

function dropdown_checkbox_tag_click(tagindex)
{
    glDropdown_checkbox_tag_click = true;
}

//
// Standard Deviation
//
function calculate_standard_deviation(filename, traceCSVArray)
{
    var tagIdx = Number(getDataIndexFromCSV('tag', traceCSVArray));
    var validIdx = Number(getDataIndexFromCSV('valid', traceCSVArray));
    var anchors = [];
    var tagSD = [[]]; // different tag for each anchor


    //console.log('calculate_standard_deviation() tagIdx:' + tagIdx + ' validIdx:' + validIdx);

    var index;
    for(index in traceCSVArray)
    {
        var data = traceCSVArray[index];


        if (index == 0)
        {
            //
            // find all anchors
            //
            var anidx = tagIdx + 1;
            for(;anidx<validIdx; anidx++)
            {
                var anchor = data[anidx];
                anchors.push(anchor);
            }
            continue;
        }

        var curTag = data[tagIdx];
        var validCount = data[validIdx];

        if (tagSD[curTag] == undefined )
        {
            console.log('calculate_standard_deviation() tagSD[curTag]:undefined curTag:' + curTag);
            tagSD[curTag] = [];
        }

        //
        // find all anchors
        //
        var anidx = tagIdx + 1;
        for(;anidx<validIdx; anidx++)
        {
            var anchor = anchors[anidx];
            var anchorDistance = data[anidx];
            if (tagSD[curTag][anchor] == undefined )
            {
                //console.log('calculate_standard_deviation() tagSD[curTag]anchor]:undefined curTag:' + curTag +", anchor:"+anchor);
                tagSD[curTag][anchor] = [];
            }

            tagSD[curTag][anchor].push(anchorDistance);
        }
    }

    console.log('calculate_standard_deviation() anchor.length:' + anchors.length +", tagSD.length:"+tagSD.length);

    var index = 0;
    for(;index<tagSD.length; index++)
    {
        var curTag = tagSD[index];
        console.log('calculate_standard_deviation() curTag:' + curTag);

        //tagSD[curTag][anchor].push(anchor);
    }



}

function getDataIndexFromCSV(patent, traceCSVArray)
{
    var index = -1;
    var headers = traceCSVArray[0];
    for( idx in headers)
    {
        if (headers[idx] == patent)
        {
            index = idx;
            break;
        }
    }
    //console.log('getDataIndexFromCSV() patent:' + patent + ' index:' + index);

    return index;
}

// function url_content(url)
// {
//     return $.get(url);
// }

function open_analyzer_page()
{
    var win = window.open('/index.php/vilsanalyzer/'+project_id, '_blank');
    win.focus();
}
