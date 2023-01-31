<?php
include ('checksession.php');
include ('config.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

      <div class="modal fade" id="modal-savebodyinfo">
        <div class="modal-dialog">
          <div class="modal-content">
            <div class="modal-header">
              <button type="button" style="display:none" data-toggle="modal" data-target="#modal-savebodyinfo" id="opensavebodyinfomodqlbtn"></button>
              <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                <span aria-hidden="true">&times;</span></button>
              <h4 class="modal-title">儲存生理量測資訊</h4>
            </div>
            <div class="modal-body">
              <div>
                <div>
                  <label class="control-label">指定標籤:</label>
                  <div class="btn-group">
                    <button type="button" class="btn btn-default btn-flat" id="target_tag"><?php echo $vilsfootprint_select_tag;?></button>
                    <button type="button" class="btn btn-default btn-flat dropdown-toggle" data-toggle="dropdown">
                      <span class="caret"></span>
                      <span class="sr-only">Toggle Dropdown</span>
                    </button>
                    <ul class="dropdown-menu" role="menu" id="dropdown_tags">
                    </ul>
                  </div>
                </div>
                <br/>
                <div>
                  <label for="bodyinfocontentName" id="bodyinfocontentNameId">心率資訊:</label>
                  <input name="bodyinfocontentName" id="bodyinfocontent" type="text" />
                </div>
               </div>
            </div>
            <div class="modal-footer">
                <div class="callout callout-warning" id="saveinfo_sending_callout" style="display:none">
                  <p>儲存資訊中...</p>
                </div>
                <div class="callout callout-danger" id="saveinfo_failed_callout" style="display:none">
                  <div id="sendAtcmd_failed_msg_callout"><p>儲存資訊 失敗!</p></div>
                </div>
                <div class="callout callout-success" id="saveinfo_finished_callout" style="display:none">
                  <p>儲存資訊 成功!</p>
                </div>

              <button type="button" class="btn btn-default" id="saveinfobtn">儲存資訊</button>
            </div>
          </div>
          <!-- /.modal-content -->
        </div>
        <!-- /.modal-dialog -->
      </div>
      <!-- /.modal -->


    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsbodyinfo_title;?></h3>
            <div class="box-tools pull-right">
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">

    		<div class="col-xs-4 col-sm-3 col-md-3 col-lg-3">

                <div class="nav-tabs-custom">
                  <ul class="nav nav-tabs">
                    <li class="active"><a href="#loc_tab_abbox" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilsbodyinfo_abbox;?></span></a></li>
                  </ul>

                  <div class="tab-content">
                     <div class="tab-pane active table-responsive" id="loc_tab_abbox">
    					<table class="table table-hover">
    						<thead>
    						</thead>
    						<tbody id="abbox_list_content">

    						</tbody>
    					</table>
                     </div>
                     <!-- /.tab-pane -->
                   </div>
                   <!-- /.tab-content -->
                </div>
                <!-- nav-tabs-custom -->

    		</div>
    		<!-- col  -->

    		<div class="col-xs-8 col-sm-9 col-md-9 col-lg-9">
              <div class="row">
                <div class="box">
                    <div class="box-header with-border">
                        <h3 class="box-title">
                            <?php echo $vilsbodyinfo_abbox;?>
                            <span class="badge bg-yellow" id="NODE_BODYINFO_TABLE_HEAD"></span>
                        </h3>
                    </div>
                    <div class="box-body no-padding">
                        <div id="NODE_BODYINFO_TABLE">
                        </div>
                        <!-- <div id="LOCATOR_BODYINFO_TABLE">
                        </div> -->
                    </div>
                   <!-- /.box-body -->
                </div>
                <!-- box  -->
             </div>
             <!-- row -->

             <div class="row" style="display:none" id="weight_chart">
                 <div class="col-xs-12 col-sm-12 col-md-10 col-lg-8">
                 <!-- solid sales graph -->
                 <div class="box box-solid bg-teal-gradient">
                       <div class="box-header">
                     <i class="fa fa-th"></i>

                     <h3 class="box-title" id="weight_chart_title">體重紀錄</h3>

                     <div class="box-tools pull-right">
                     </div>
                   </div>
                   <div class="box-body border-radius-none">
                     <div class="chart" style="height: 150px;" id="weight_line_chart"></div>
                   </div>
                   <!-- /.box-body -->
                   <div class="box-footer no-border">
                     <div class="row">
                       <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                         <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                data-fgColor="#39CCCC" id="weight_knob_3">

                         <div class="knob-label" id="weight_label_3"></div>
                       </div>
                       <!-- ./col -->
                       <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                         <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                data-fgColor="#CC1212" id="weight_knob_2">

                         <div class="knob-label" id="weight_label_2"></div>
                       </div>
                       <!-- ./col -->
                       <div class="col-xs-4 text-center">
                         <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                data-fgColor="#39CCCC" id="weight_knob_1">

                         <div class="knob-label" id="weight_label_1"></div>
                       </div>
                       <!-- ./col -->
                     </div>
                     <!-- /.row -->
                   </div>
                   <!-- /.box-footer -->
                 </div>
                 <!-- /.box -->
                 </div>
             </div>
             <!-- row -->


              <div class="row" style="display:none" id="heartbeat_chart">
                  <div class="col-xs-12 col-sm-12 col-md-10 col-lg-8">
                  <!-- solid sales graph -->
                  <div class="box box-solid bg-aqua">
                    <div class="box-header">
                      <i class="fa fa-th"></i>

                      <h3 class="box-title" id="heartbeat_chart_title">心率紀錄</h3>

                      <div class="box-tools pull-right">
                      </div>
                    </div>
                    <div class="box-body border-radius-none">
                      <div class="chart" style="height: 150px;" id="heartbeat_line_chart"></div>
                    </div>
                    <!-- /.box-body -->
                    <div class="box-footer no-border">
                      <div class="row">
                        <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                          <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                 data-fgColor="#39CCCC" id="heartbeat_knob_3">

                          <div class="knob-label" id="heartbeat_label_3"></div>
                        </div>
                        <!-- ./col -->
                        <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                          <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                 data-fgColor="#CC1212" id="heartbeat_knob_2">

                          <div class="knob-label" id="heartbeat_label_2"></div>
                        </div>
                        <!-- ./col -->
                        <div class="col-xs-4 text-center">
                          <input type="text" class="knob" data-readonly="true" value="0" data-max="200" data-min="0" data-width="60" data-height="60"
                                 data-fgColor="#39CCCC" id="heartbeat_knob_1">

                          <div class="knob-label" id="heartbeat_label_1"></div>
                        </div>
                        <!-- ./col -->
                      </div>
                      <!-- /.row -->
                    </div>
                    <!-- /.box-footer -->
                  </div>
                  <!-- /.box -->
                  </div>
              </div>
              <!-- row -->


           </div>

        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->


   <div style="text-align:center">
       <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
   </div>

    <script>
      var project_id = 1;
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfo.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/bodyinfo_src/00-raphael.js"></script>
    <script src="/js/bodyinfo_src/01-morris.js"></script>
    <script src="/js/bodyinfo_src/02-jquery.knob.js"></script>
    <script src="/js/bodyinfo_src/03-bodyinfo.js"></script> -->

</html>
