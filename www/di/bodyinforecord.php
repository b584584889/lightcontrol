<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsbodyinfo_record;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_bodyinfo_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">

    		<div class="col-xs-12 col-sm-12 col-md-12 col-lg-12">
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

                  <div class="col-xs-8 col-sm-8 col-md-8 col-lg-8">
                      <span class='pull-left' id="pdf_report_status"></span>
                      <button type='button' class='btn btn-app pull-left' onclick='get_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>
                  </div>


                </div>
                <!-- row -->

                <div class="row">
                    <div id='show_chart_area'>

                    </div>
                </div>
                <!-- row -->

           </div>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = 1;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinforecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/bodyinforecord_src/00-raphael.js"></script>
    <script src="/js/bodyinforecord_src/01-morris.js"></script>
    <script src="/js/bodyinforecord_src/02-jquery.knob.js"></script>
    <script src="/js/bodyinforecord_src/03-daterangepicker"></script>
    <script src="/js/bodyinforecord_src/04-bodyinforecord.js"></script> -->

</html>
