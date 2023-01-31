<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - 2D Map</title>
  </head>

    <div class="modal fade" id="modal_fixposition">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" style="display:none" data-toggle="modal" data-target="#modal_fixposition" id="openfixpositionmodalbtn"></button>
          <button type="button" class="close" data-dismiss="modal" aria-label="Close">
            <span aria-hidden="true">&times;</span></button>
          <h4 class="modal-title">位置校正</h4>
        </div>
        <div class="modal-body">

            <label class="control-label">最新測距資料所得座標與顯示座標差異超過50cm之所有Tag</label>

            <div style="height:300px;overflow:scroll">
            <table class="table table-bordered">
                <thead>
                    <tr style="background-color:#FFFFFF;">
                        <th style="width:10px;background-color:#EEEEEE;">#</th>
                        <th style="width:120px;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                        <th style="width:90px;background-color:#EEEEEE;">位置差距</th>
                        <th style="background-color:#EEEEEE;">處理方式</th>
                    </tr>
                </thead>
                <tbody id="tag_list_content_fixposition">

                </tbody>
            </table>
            </div>

        </div>
        <div class="modal-footer">
            <div class="callout callout-warning" id="fixposition_sending_callout" style="display:none">
              <p>更新中...</p>
            </div>
            <div class="callout callout-danger" id="fixposition_failed_callout" style="display:none">
              <div id="fixposition_failed_msg_callout"><p>更新失敗!</p></div>
            </div>
            <div class="callout callout-success" id="fixposition_finished_callout" style="display:none">
              <p>更新 成功!</p>
            </div>
            <div class="callout callout-info" id="fixposition_reponse_msg_callout" style="display:none">
              <p></p>
            </div>

          <button type="button" class="btn btn-default" id="fixposition_updatebtn" onclick="fixposition_update();">更新</button>
        </div>
      </div>
      <!-- /.modal-content -->
    </div>
    <!-- /.modal-dialog -->
    </div>
    <!-- /.modal -->


    <div class="modal fade" id="modal_fixdevice">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" style="display:none" data-toggle="modal" data-target="#modal_fixdevice" id="openfixdevicemodalbtn"></button>
          <button type="button" class="close" data-dismiss="modal" aria-label="Close">
            <span aria-hidden="true">&times;</span></button>
          <h4 class="modal-title">裝置校正</h4>
        </div>
        <div class="modal-body">

            <label class="control-label">列出最新無活動坐標標籤與系統判斷狀態</label>

            <div style="height:300px;overflow:scroll">
            <table class="table table-bordered">
                <thead>
                    <tr style="background-color:#FFFFFF;">
                        <th style="width:10px;background-color:#EEEEEE;">#</th>
                        <th style="width:120px;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                        <th style="width:90px;background-color:#EEEEEE;">狀態</th>
                        <th style="background-color:#EEEEEE;">處理方式</th>
                    </tr>
                </thead>
                <tbody id="tag_list_content_fixdevice">

                </tbody>
            </table>
            </div>

        </div>
        <div class="modal-footer">
            <div class="callout callout-warning" id="fixdevice_sending_callout" style="display:none">
              <p>更新中...</p>
            </div>
            <div class="callout callout-danger" id="fixdevice_failed_callout" style="display:none">
              <div id="fixdevice_failed_msg_callout"><p>更新失敗!</p></div>
            </div>
            <div class="callout callout-success" id="fixdevice_finished_callout" style="display:none">
              <p>更新 成功!</p>
            </div>
            <div class="callout callout-info" id="fixdevice_reponse_msg_callout" style="display:none">
              <p></p>
            </div>

          <button type="button" class="btn btn-default" id="fixdevice_updatebtn" onclick="fixdevice_update();">更新</button>
        </div>
      </div>
      <!-- /.modal-content -->
    </div>
    <!-- /.modal-dialog -->
    </div>
    <!-- /.modal -->



    <!--
        areaname edit
    -->
      <div class="row" id="areaname_edit" style="display: none;">
        <div class="col-md-6">
          <div class="info-box">
            <span class="info-box-icon bg-aqua"><i class="fa fa-object-group"></i></span>

            <div class="info-box-content">
              <h5 class="box-title"><?php echo $vils2dmap_edit_area_name;?></h5>
              <div class="input-group margin">
                  <div class="input-group-btn" id="area_type_selection">
                      <button type="button" class="btn btn-default" data-id="0" id="area_type_show"><?php echo $vils2dmap_edit_area_type_0;?></button>
                      <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                          <span class="caret"></span>
                          <span class="sr-only">Toggle Dropdown</span>
                      </button>
                      <ul class="dropdown-menu" role="menu">
                          <li><a href="javascript:;" onclick="area_type_select(0);return false;" data-id="0" id="area_type_0"><?php echo $vils2dmap_edit_area_type_0;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(1);return false;" data-id="1" id="area_type_1"><?php echo $vils2dmap_edit_area_type_1;?></a></li>
                          <li style="display:none;"><a href="javascript:;" onclick="area_type_select(2);return false;" data-id="2" id="area_type_2"><?php echo $vils2dmap_edit_area_type_2;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(3);return false;" data-id="3" id="area_type_3"><?php echo $vils2dmap_edit_area_type_3;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(4);return false;" data-id="4" id="area_type_4"><?php echo $vils2dmap_edit_area_type_4;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(5);return false;" data-id="5" id="area_type_5"><?php echo $vils2dmap_edit_area_type_5;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(6);return false;" data-id="6" id="area_type_6"><?php echo $vils2dmap_edit_area_type_6;?></a></li>
                          <li><a href="javascript:;" onclick="area_type_select(999);return false;" data-id="999" id="area_type_999"><?php echo $vils2dmap_edit_area_type_999;?></a></li>
                      </ul>
                  </div>
                <div class="input-group-btn">
                  <input type="hidden" id="areaname_color" class="form-control colorpicker" data-opacity=".5" value="#db913d">
                  <!-- <input type="text" id="areaname_color" class="form-control colorpicker" data-opacity=".5" value="#db913d"> -->
                </div>
                <div class="input-group-btn">
                    <input type="text" class="form-control" style="width:100px;" id="areacolor_input">
                </div>
                <input type="text" class="form-control" id="areaname_input">
                <div class="input-group-btn">
                  <button type="button" class="btn btn-info btn-flat" onclick="areaname_save();return false;"><?php echo $vils2dmap_edit_save;?></button>
                </div>
              </div>
            </div>
            <!-- /.info-box-content -->
          </div>
          <!-- /.info-box -->
        </div>
        <!-- /.col-md-12 -->
      </div>
      <!-- /.row -->



        <div class="row">
          <div class="col-md-12">

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vils2dmap_title;?></h3>
                <div class="box-tools pull-right">

                    <div class="btn-group">
                        <input type="text" id="map_search_text" onfocus="map_search_text_onfocus(this.id)" placeholder="尋找裝置">
                        <ul class="dropdown-menu" id="dropdown_searchresult">
                        </ul>
                    </div>

                    <div class="btn-group">
                        <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vils2dmap_show_locators;?><span class="caret"></span></button>
                        <ul class="dropdown-menu" id="dropdown_locators">
                        </ul>
                    </div>
                    <div class="btn-group">
                        <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vils2dmap_show_tags;?><span class="caret"></span></button>
                        <ul class="dropdown-menu" id="dropdown_tags">
                        </ul>
                    </div>

                  <!-- <a href="javascript:void(0)" onclick="open_2dmap_page();return false;"><i class="fa fa-external-link"></i></a> -->
                  <!-- <a href="#control-sidebar-settings-tab" data-toggle="control-sidebar" onclick="toggleOn();"><i class="fa fa-gears"></i></a> -->
                  <button type="button" class="btn btn-box-tool" onclick="open_2dmap_page();return false;"><i class="fa fa-external-link"></i>
                  </button>


                    <?php
                        //if ($_SESSION["groupid"] == '1' || $_SESSION["groupid"] == '2')
                        //{
                        //    echo "    <button type='button' class='btn btn-box-tool' data-toggle='control-sidebar' onclick='toggleOn();'><i class='fa fa-gears'></i>";
                        //    echo "    </button>";
                        //}
                    ?>

                  <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                  </button>
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body no-padding">
                <!-- <div class="row"> -->
                  <!-- Custom Tabs (Pulled to the right) -->
                  <div class="nav-tabs-custom">

                    <ul class="nav nav-tabs" id="map_nav_tabs">
                        <!--

                        Show PathGroup Info Here

                        -->
                    </ul>
                    <div class="tab-content" id="map_tab_content">
                        <!--

                        Show All Path Info Here

                        -->
                    </div>
                    <!-- /.tab-content -->

                  </div>
                  <!-- nav-tabs-custom -->
                <!-- </div> -->

                <!-- <div class="row">
                  <div class="col-md-12 col-sm-12">
                    <div class="pad">
                      <div id="mapid" style="height:85vh"></div>
                    </div>
                  </div>
                </div> -->

              </div>
              <!-- /.box-body -->
              <div class="box-footer">
                  <div class="btn-group">
                      <div class="checkbox">
                        <label>
                          <input type="checkbox" id="showtrack"> Show Track
                        </label>
                      </div>
                      <div class="checkbox">
                        <label>
                          <input type="checkbox" id="uismoothingcheckbtn" checked="true"> UI Smoothing
                        </label>
                      </div>
                  </div>

              <?php
                if ($GL_CONFIG_EDIT_2DMAP)
                {
                    echo "<div class='btn-toolbar' role='toolbar'>\n";
                    echo "    <div class='btn-group'>\n";
                    echo "      <button type='button' class='btn btn-success' role='group' id='fixpositionbtn' onclick='FixPosition();return false;'>位置校正</button>\n";
                    echo "  </div>\n";
                    echo "  <div class='btn-group'>\n";
                    echo "      <button type='button' class='btn btn-primary' role='group' id='fixdevice' onclick='FixDevice();return false;'>裝置校正</button>\n";
                    echo "    </div>\n";
                    echo "</div>\n";
                    echo "<br/>\n";
                    echo "<div class='btn-group'>\n";
                    echo "    <button type='button' class='btn btn-danger pull-left' id='dumprangingresultbtn' onclick='DumpRangingResult();return false;'>Dump Ranging Result(10 Seconds)</button>\n";
                    echo "    <input type='text' style='width:50px' id='dumprangingresulttimeout' value='10'>\n";
                    echo "    <a href='javascript:void(0)' onclick='open_log_page();return false;'><i class='fa fa-external-link'></i></a>\n";
                    echo "</div>\n";
                    echo "  <p>\n";
                    echo "    <br/>\n";
                    echo "    <button type='button' class='btn btn-info pull-left' id='dumpaoaranginglogbtn' onclick='DumpAoARangingLog();return false;'>Dump AoA Ranging Log Once</button>\n";
                    echo "  </p>\n";
                }
                ?>

                 </div>
              </div>
            </div>
            <!-- /.box box-success -->
            <!-- /.MAP & BOX PANE -->
          </div>
          <!-- /.col-md-12 -->
        </div>
        <!-- /.row -->


    <!-- Control Sidebar -->
    <aside class="control-sidebar control-sidebar-dark" style="display:none;" id="changemap_aside_apnel">
      <!-- Create the tabs -->
      <ul class="nav nav-tabs nav-justified control-sidebar-tabs">
        <li><a href="#control-sidebar-settings-tab" data-toggle="control-sidebar" onclick="toggleOff();"><i class="fa fa-gears"></i></a></li>
      </ul>

      <!-- Tab panes -->
      <div class="tab-content">
       </div>
    </aside>
    <!-- /.control-sidebar -->
    <!-- Add the sidebar's background. This div must be placed
         immediately after the control sidebar -->
    <div class="control-sidebar-bg"></div>


  </div>
  <!-- ./wrapper -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";

      var show_nodeid = "<?php echo $nodeid ?>";
      var GL_CONFIG_EDIT_2DMAP = <?php echo $GL_CONFIG_EDIT_2DMAP ?>;
      var GL_CONFIG_2DMAP_SHOW_GMAP = <?php echo $GL_CONFIG_2DMAP_SHOW_GMAP ?>;

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet_draw.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_live.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/map_live_src/00-Leaflet.ImageOverlay.Rotated.js"></script>
    <script src="/js/map_live_src/01-mapapi.js"></script>
    <script src="/js/map_live_src/02-Indoor2DMap.js"></script>
    <script src="/js/map_live_src/03-LiveMap.js"></script>
    <script src="/js/map_live_src/04-fixnode.js"></script> -->

    <script>
    function toggleOn()
    {
        $('#changemap_aside_apnel').css('display','block');
    }
    function toggleOff()
    {
        $('#changemap_aside_apnel').css('display','none');
    }
    </script>

</html>
