<?php
include ('checksession.php');
include ('config.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $billboardrecords_title;?></h3>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
          </div>

          <div class="btn-group">
              <input type="text" id="search_billboard_title_text" placeholder="尋找公告標題">
          </div>
          <div class="btn-group">
              <input type="text" id="search_billboard_date_text" placeholder="日期:YYYY-MM-DD">
          </div>

        </div>
        <!-- /.box-header -->
        <div class="box-body">

            <div style="height:650px;overflow:scroll;">
                <table class="table table-bordered">
                    <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 10px">#</th>
                            <th style="width: 180px">標題</th>
                            <th>內容</th>
                            <th style="width: 80px">公告對象</th>
                            <th style="width: 100px">公告時間</th>
                        </tr>
                    </thead>
                    <tbody id="table_content">
                    </tbody>
                </table>
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
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/billboardrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/billboardrecords_src/00-billboardrecords.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
