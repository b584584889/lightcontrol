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
    <title>SMIMS VILS IoT - Power Meter Monitor</title>
    <style>
        thead tr {
            background-color:rgb(236, 240, 245);
        }

        .listItem {
            background-color: #FFF;
            cursor: pointer;
        }
        .listItemSelected {
            background-color: #CCEEFF;
        }

    </style>
  </head>

  <div class="box" >
      <div class="box-header with-border">
        <h3 class="box-title"><?php echo $vilsevnetreport_title2;?></h3>
        <div class="box-tools pull-right">
          <ul class="pagination pagination-sm no-margin pull-right">
            <li>
              <button type="button" class="btn btn-box-tool" onclick="open_powermeterview_page();return false;">
                <i class="fa fa-external-link"></i>
              </button>
            </li>
          </ul>
        </div>
      </div>
      <div class="box-body">
          <div class="row">
              <div class="col-xs-4 col-sm-3 col-md-3 col-lg-3">
                  <div class="nav-tabs-custom">
                    <ul class="nav nav-tabs">
                      <li class="active"><a href="#pm_tab_id" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilspowermeterview_pm_tag;?></span></a></li>
                      <!-- <li><a href='#loc_tab_user' data-toggle='tab'><span class='badge bg-aqua-active'><?php echo $vilspushnotification_user;?></span></a></li> -->
                      <li><a href="#pm_tab_search" data-toggle="tab">搜尋</a></li>
                    </ul>
                    <div class="tab-content">
                       <div class="tab-pane active table-responsive" id="pm_tab_id">
                           <div style="height:650px;overflow:scroll">
                              <table class="table table-hover">
                                 <thead>
                                     <tr style="background-color:#FFFFFF;">
                                       <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                       <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspowermeterview_pm_id;?></th>
                                     </tr>
                                 </thead>
                                 <tbody id="pm_list_content">
                                 </tbody>
                             </table>
                          </div>
                       </div>
                       <div class="tab-pane table-responsive" id="pm_tab_search">
                          <table class="table table-hover">
                              <thead>
                                  <input type="text" id="pm_tab_search_text">
                              </thead>
                              <tbody id="search_list_content" />
                          </table>
                       </div>
                    </div>
                  </div>
              </div>
              <div class="col-xs-8 col-sm-9 col-md-9 col-lg-9">
                  <div class="row">
                      <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">
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
                      </div>

                  </div>
                  <div class="row">
                      <div id='div_pm_chart_area'></div>
                  </div>
              </div>
      </div>
      </div>
  </div>

  <script>
    var project_id = <?php echo $project_id ?>;
    var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
    var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
    var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/powermeterview_src/03-raphael.js?s=1"></script>
  <script src="/js/powermeterview_src/02-daterangepicker.js?date=<?php echo $UPDATE_DATE; ?>"></script>
  <script src="/js/powermeterview_src/01-morris.js?date=<?php echo $UPDATE_DATE; ?>"></script>
  <script src="/js/powermeterview_src/00-powermeterview.js?date=<?php echo $UPDATE_DATE; ?>"></script>

</html>
