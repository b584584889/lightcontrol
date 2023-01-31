<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="col-md-12 col-sd-12">
      <!-- Event Settings -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsinspectionpath_record_title;?></h3>
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

      <div style="text-align:center">
          <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
      </div>

    <script>
        var project_id = 1;
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
