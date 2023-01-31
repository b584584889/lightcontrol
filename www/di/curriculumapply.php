<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="wrapper">
    <div class="content-wrapper" style="margin:0; padding:0; border:0 none;">
    <section class="content" style="margin:0; padding:0; border:0 none;">

      <div class="col-md-12 col-sd-12">
          <div class="row">

              <div class="box">
                <div class="box-header with-border">
                  <h3 class="box-title">選課課程</h3>

                  <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                        <li><a href="#" id="id_current_page_num">1</a></li>
                        <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                      </ul>
                  </div>
                </div>
                <!-- /.box-header -->
                <div class="box-body table-responsive">

                  <table class="table table-bordered" id="CURRICULUM_RECORD_TABLE">
                    <thead>
                      <tr style="background-color:rgb(236, 240, 245);">
                        <th style="width: 30px">#</th>
                        <th style="width: 100px">課程類別[課程號碼]</th>
                        <th style="width: 100px">課程名稱</th>
                        <th>課程日期</th>
                        <th>課程時間</th>
                        <th>課程週間日</th>
                        <th>總時數(小時)</th>
                        <th>地點</th>
                        <th>內容</th>
                        <th>任課老師</th>
                        <th>任課老師簡介</th>
                        <th>費用</th>
                        <th>備註</th>
                        <th>簡章下載</th>
                        <th>狀態</th>
                        <th>操作</th>
                      </tr>
                    </thead>
                    <tbody id="CURRICULUM_RECORD_TABLE_BODY">

                    </tbody>
                  </table>

                </div>
                <!-- /.box-body -->
              </div>
              <!-- /.box -->

          </div>
          <!-- /.row -->

      </div>
      <!-- /.col -->

  </section>
  </div>
  <!-- content-wrapper -->
  </div>
  <!-- ./wrapper -->

      <div style="text-align:center">
          <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
      </div>

    <script>
        var project_id = 1;
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/curriculumapply.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/curriculumapply_src/00-curriculumapply.js"></script> -->
</html>
