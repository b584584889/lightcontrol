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
    <title>SMIMS VILS IoT - Health Examination Record</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $left_side_bar_bodyinfo_record;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_bodyinfo_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">

    		<div class="col-xs-4 col-sm-3 col-md-3 col-lg-3">

                <div class="nav-tabs-custom">
                  <ul class="nav nav-tabs">
                    <li class="active"><a href="#loc_tab_tag" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilsdevices_tag;?></span></a></li>
                    <li><a href='#loc_tab_user' data-toggle='tab'><span class='badge bg-aqua-active'><?php echo $vilspushnotification_user;?></span></a></li>
                    <li><a href="#loc_tab_search" data-toggle="tab">搜尋</a></li>
                  </ul>

                  <div class="tab-content">
                     <div class="tab-pane active table-responsive" id="loc_tab_tag">
    					<table class="table table-hover">
    						<thead>
    						</thead>
    						<tbody id="tag_list_content">

    						</tbody>
    					</table>
                     </div>
                     <!-- /.tab-pane -->

                     <div class="tab-pane active table-responsive" id="loc_tab_user">
                         <div style="height:650px;overflow:scroll">
     					<table class="table table-hover">
     						<thead>
     							<tr style="background-color:#FFFFFF;">
     							  <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                  <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_username;?></th>
     							</tr>
     						</thead>
     						<tbody id="user_list_content">

     						</tbody>
     					</table>
                         </div>
                     </div>
                      <!-- /.tab-pane -->

                     <div class="tab-pane table-responsive" id="loc_tab_search">
    					<table class="table table-hover">
    						<thead>
                                <input type="text" id="loc_tab_search_text">
    						</thead>
    						<tbody id="search_list_content">

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

                  <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">
                        <!-- Date and time range -->
                        <div class="form-group">
                          <label>選擇查詢區間:</label>

                          <div class="input-group">
                            <button type="button" class="btn btn-default pull-right" id="daterange-btn">
                              <span>
                                <i class="fa fa-calendar"></i> 查詢區間
                              </span>
                              <i class="fa fa-caret-down"></i>
                            </button>
                          </div>
                        </div>
                        <!-- /.form group -->
                  </div>

                  <!-- <div class="col-xs-8 col-sm-8 col-md-8 col-lg-8">
                      <span class='pull-left' id="pdf_report_status"></span>
                      <button type='button' class='btn btn-app pull-left' onclick='get_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>
                  </div> -->


                </div>
                <!-- row -->

                <div class="row">
                    <div id='show_chart_area'>

                    </div>
                </div>
                <!-- row -->

             <!-- <div class="row" style="display:none" id="weight_chart">
                 <div class="col-xs-12 col-sm-12 col-md-10 col-lg-8">
                 <div class="box box-solid bg-teal-gradient">
                    <div class="box-header">
                     <i class="fa fa-th"></i>
                     <h3 class="box-title" id="weight_chart_title">體重紀錄</h3>
                   </div>
                   <div class="box-body border-radius-none">
                     <div class="chart" style="height: 150px;" id="weight_chart_line_chart"></div>
                   </div>
                 </div>
                 </div>
             </div> -->



              <!-- <div class="row" style="display:none" id="heartbeat_chart">
                  <div class="col-xs-12 col-sm-12 col-md-10 col-lg-8">
                  <div class="box box-solid bg-aqua">
                    <div class="box-header">
                      <i class="fa fa-th"></i>
                      <h3 class="box-title" id="heartbeat_chart_title">心率紀錄</h3>
                    </div>
                    <div class="box-body border-radius-none">
                      <div class="chart" style="height: 150px;" id="heartbeat_chart_line_chart"></div>
                    </div>
                  </div>
                  </div>
              </div> -->



           </div>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/bodyinforecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/bodyinforecord_src/00-raphael.js"></script>
    <script src="/js/bodyinforecord_src/01-morris.js"></script>
    <script src="/js/bodyinforecord_src/02-jquery.knob.js"></script>
    <script src="/js/bodyinforecord_src/03-daterangepicker"></script>
    <script src="/js/bodyinforecord_src/04-bodyinforecord.js"></script>

</html>
