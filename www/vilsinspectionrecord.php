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
    <title>SMIMS VILS IoT - <?php echo $vilsinspectionpath_record_title;?></title>
  </head>

    <div class="col-md-12 col-sd-12">
      <!-- Event Settings -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsinspectionpath_record_title;?></h3>
          <div class="box-tools pull-right">
              <button type="button" class="btn btn-box-tool" onclick="open_inspection_record_page();return false;"><i class="fa fa-external-link"></i>
              </button>
              <!-- <a href="javascript:void(0)" onclick="open_sections_page();return false;"><i class="fa fa-external-link"></i></a> -->
          </div>
        </div>
        <!-- /.box-header -->

          <div class="box-body">
              <!-- Custom Tabs (Pulled to the right) -->
              <div class="nav-tabs-custom">

                <ul class="nav nav-tabs" id="inspectiongroup_nav_tabs">
                    <!--

                    Show PathGroup Info Here

                    -->
                </ul>
                <div class="tab-content" id="inspectiongroup_tab_content">
                    <!--

                    Show All Path Info Here

                    -->
                </div>
                <!-- /.tab-content -->

              </div>
              <!-- nav-tabs-custom -->
              <div id="INSPECTIONRECORD_TABLE_MSG"><?php echo $vilsinspectionrecord_loading;?></div>
          </div>
          <!-- /.box-body -->
        </div>
        <!-- /.box-->
      </div>
      <!-- /.col -->

    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/inspectionrecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/locales/bootstrap-datepicker.zh-TW.min.js"></script>

    <!-- <script src="/js/inspectionrecord_src/00-raphael.js"></script>
    <script src="/js/inspectionrecord_src/01-morris.js"></script>
    <script src="/js/inspectionrecord_src/02-bootstrap-datepicker.js"></script>
    <script src="/js/inspectionrecord_src/03-inspectionrecord.js"></script> -->

</html>
