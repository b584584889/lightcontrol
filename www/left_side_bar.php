<!-- Left side column. contains the logo and sidebar -->
<aside class="main-sidebar">

  <!-- sidebar: style can be found in sidebar.less -->
  <section class="sidebar">

    <!-- Sidebar Menu -->
    <ul class="sidebar-menu">

      <!-- Devices -->
      <li class="header">

          <?php

            if ($_SESSION['groupid'] != 6)
            {
                echo $left_side_bar_devices;
            }

          ?>

      </li>

      <?php
        if ($GL_CONFIG_ENABLE_DASHBOARD == 1)
        {
            echo "<li class='treeview "; if($dashboard_active) echo 'active'; echo "' id='treeview_dashboard'>";
            echo "  <a href='javascript:void(0)' onclick='show_dashboard("; echo $project_id; echo ");return false;'><i class='fa fa-bar-chart'></i> <span>"; echo $left_side_bar_dashboard_list; echo "</span></a>\n";
            echo "</li>\n";
        }
      ?>

      <?php
        if ($GL_CONFIG_ENABLE_NODELIST == 1 || $_SESSION['type'] == 2)
        {
            echo "<li class='treeview "; if($vils_active) echo 'active'; echo "' id='treeview_devices'>";
            echo "  <a href='javascript:void(0)' onclick='show_devices("; echo $project_id; echo ");return false;'><i class='fa fa-users'></i> <span>"; echo $left_side_bar_devices_list; echo "</span></a>";
            echo "</li>\n";
        }
      ?>

      <?php
        if ($GL_CONFIG_ENABLE_RFIDDEVICELIST == 1)
        {
            echo "      <li class='treeview "; if($rfiddevicelist_active) echo 'active'; echo "' id='treeview_rfiddevices'>";
            echo "        <a href='#'>\n";
            echo "          <i class='fa fa-rss'></i> <span>"; echo $left_side_bar_rfiddevices_list; echo "</span>";
            echo "          <span class='pull-right-container'>";
            echo "            <i class='fa fa-angle-left pull-right'></i>";
            echo "          </span>\n";
            echo "        </a>\n";
            echo "        <ul class='treeview-menu'>\n";
            echo "          <li ><a href='javascript:void(0)' onclick='show_rfiddevices("; echo $project_id; echo ");return false;'><i class='fa fa-rss'></i> <span>"; echo $left_side_bar_rfiddevices; echo "</span></a></li>";
            echo "          <li ><a href='javascript:void(0)' onclick='show_rfidcards("; echo $project_id; echo ");return false;'><i class='fa fa-tags'></i> <span>"; echo $left_side_bar_rfidcards; echo "</span></a></li>";
            echo "        </ul>\n";
            echo "      </li>\n";
        }
      ?>

    <?php
      if ($GL_CONFIG_ENABLE_USEINFO == 1)
      {
          echo "      <li class='treeview "; if($userinfo_active) echo 'active'; echo "' id='treeview_userinfo'>";
          echo "        <a href='#'>\n";
          echo "          <i class='fa fa-user'></i> <span>"; echo $left_side_bar_userinfo; echo "</span>";
          echo "          <span class='pull-right-container'>";
          echo "            <i class='fa fa-angle-left pull-right'></i>";
          echo "          </span>\n";
          echo "        </a>\n";
          echo "        <ul class='treeview-menu'>\n";
          echo "          <li ><a href='javascript:void(0)' onclick='show_userinfo("; echo $project_id; echo ");return false;'><i class='fa fa-user'></i> <span>"; echo $left_side_bar_userlist; echo "</span></a></li>";
          if ( $_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 )
          {
          echo "          <li ><a href='javascript:void(0)' onclick='show_userregister("; echo $project_id; echo ");return false;'><i class='fa fa-users'></i> <span>"; echo $left_side_bar_userregister; echo "</span></a></li>";
          echo "          <li ><a href='javascript:void(0)' onclick='show_usergroupinfo("; echo $project_id; echo ");return false;'><i class='fa fa-users'></i> <span>"; echo $left_side_bar_usergroupinfo; echo "</span></a></li>";
          }

          if ( $GL_CONFIG_ENABLE_MAINGROUP == 1 && ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 6) )
          {
              echo "          <li ><a href='javascript:void(0)' onclick='show_usermaingroupinfo("; echo $project_id; echo ");return false;'><i class='fa fa-object-group'></i> <span>"; echo $left_side_bar_usermaingroupinfo; echo "</span></a></li>";
          }

          if ( $GL_CONFIG_ENABLE_SUBGROUP == 1)
          {
              if ( $GL_CONFIG_UNLOCK_SUBGROUP == 1 && ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION['type'] == 2) )
              // if ( $GL_CONFIG_UNLOCK_SUBGROUP == 1)
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_usersubgroupinfo("; echo $project_id; echo ");return false;'><i class='fa fa-object-group'></i> <span>"; echo $left_side_bar_usersubgroupinfo; echo "</span></a></li>";
              }
              else
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_usersubgroup_unlockpage("; echo $project_id; echo ");return false;'><i class='fa fa-object-group'></i> <span>"; echo $left_side_bar_usersubgroupinfo; echo "</span></a></li>";
              }
          }

          echo "        </ul>\n";
          echo "      </li>\n";
      }
    ?>

    <?php
      if ($GL_CONFIG_ENABLE_CARDROLLCALL == 1)
      {
          echo "      <li class='treeview "; if($rfidrollcall_active) echo 'active'; echo "' id='treeview_cardrollcall'>";
          echo "        <a href='#'>\n";
          echo "          <i class='fa fa-tags'></i> <span>"; echo $left_side_bar_cardrollcall; echo "</span>";
          echo "          <span class='pull-right-container'>";
          echo "            <i class='fa fa-angle-left pull-right'></i>";
          echo "          </span>\n";
          echo "        </a>\n";
          echo "        <ul class='treeview-menu'>\n";
          if ( $GL_CONFIG_UNLOCK_CARDROLLCALL == 1 || $_SESSION['type'] == 2)
          {
          echo "          <li ><a href='javascript:void(0)' onclick='show_carddrollcall_schedule("; echo $project_id; echo ");return false;'><i class='fa fa-calendar'></i> <span>"; echo $left_side_bar_cardrollcall_schedule; echo "</span></a></li>";
          echo "          <li ><a href='javascript:void(0)' onclick='show_cardrollcall_event("; echo $project_id; echo ");return false;'><i class='fa fa-list-alt'></i> <span>"; echo $left_side_bar_cardrollcall_event; echo "</span></a></li>";
          }
          else
          {
          echo "          <li ><a href='javascript:void(0)' onclick='show_carddrollcall_schedule_unlockpage("; echo $project_id; echo ");return false;'><i class='fa fa-calendar'></i> <span>"; echo $left_side_bar_cardrollcall_schedule; echo "</span></a></li>";
          echo "          <li ><a href='javascript:void(0)' onclick='show_cardrollcall_event_unlockpage("; echo $project_id; echo ");return false;'><i class='fa fa-list-alt'></i> <span>"; echo $left_side_bar_cardrollcall_event; echo "</span></a></li>";
          }

          if ( $_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 4 )
          {
          echo "          <li ><a href='javascript:void(0)' onclick='show_cardrollcall_record("; echo $project_id; echo ");return false;'><i class='fa fa-th-list'></i> <span>"; echo $left_side_bar_cardrollcall_record; echo "</span></a></li>";
          echo "          <li ><a href='javascript:void(0)' onclick='show_userrollcall_record("; echo $project_id; echo ");return false;'><i class='fa fa-street-view'></i> <span>"; echo $left_side_bar_userrollcall_record; echo "</span></a></li>";
          }
          echo "        </ul>\n";
          echo "      </li>\n";

          if ($GL_CONFIG_ENABLE_NFC_SCANRESULT == 1 || $GL_CONFIG_ENABLE_UHF_SCANRESULT == 1)
          {
          echo " <li class='treeview "; if($cardscanresult_active) echo 'active'; echo "' id='treeview_cardscanresult'>\n";
          echo "        <a href='#'>\n";
          echo "          <i class='fa fa-feed'></i> <span>"; echo $left_side_bar_cardscan_result; echo "</span>";
          echo "          <span class='pull-right-container'>";
          echo "            <i class='fa fa-angle-left pull-right'></i>";
          echo "          </span>\n";
          echo "        </a>\n";
          echo "        <ul class='treeview-menu'>\n";
          if ($GL_CONFIG_ENABLE_NFC_SCANRESULT == 1)
          {
              if ($GL_CONFIG_UNLOCK_NFC_SCANRESULT == 1 || $_SESSION['type'] == 2)
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_nfc_cardscan_result("; echo $project_id; echo ");return false;'><i class='fa fa-feed'></i> <span>"; echo $left_side_bar_nfc_cardscan_result; echo "</span></a></li>";
              }
              else
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_nfc_cardscan_result_unlockpage("; echo $project_id; echo ");return false;'><i class='fa fa-feed'></i> <span>"; echo $left_side_bar_nfc_cardscan_result; echo "</span></a></li>";
              }
          }

          if ($GL_CONFIG_ENABLE_UHF_SCANRESULT == 1)
          {
              if ($GL_CONFIG_UNLOCK_UHF_SCANRESULT == 1 || $_SESSION['type'] == 2)
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_uhf_cardscan_result("; echo $project_id; echo ");return false;'><i class='fa fa-feed'></i> <span>"; echo $left_side_bar_uhf_cardscan_result; echo "</span></a></li>";
              }
              else
              {
                  echo "          <li ><a href='javascript:void(0)' onclick='show_nfc_cardscan_result_unlockpage("; echo $project_id ; echo ");return false;'><i class='fa fa-feed'></i> <span>"; echo $left_side_bar_uhf_cardscan_result; echo "</span></a></li>";
              }
          }
          echo "        </ul>\n";
          }

          // echo "    <a href='javascript:void(0)' onclick='show_cardscan_result("; echo $project_id; echo ");return false;'><i class='fa fa-feed'></i> <span>"; echo $left_side_bar_cardscan_result; echo "</span></a>\n";
          echo " </li>\n";

      }
    ?>

    <?php
        if ($GL_CONFIG_ENABLE_BODYINFO == 1)
        {
            echo "      <li class='treeview "; if($bodyinfo_active) echo 'active'; echo "' id='treeview_bodyinfo'>\n";
            echo "        <a href='#'>\n";
            echo "          <i class='fa fa-heartbeat'></i> <span>"; echo $left_side_bar_bodyinfo; echo "</span>\n";
            echo "          <span class='pull-right-container'>\n";
            echo "            <i class='fa fa-angle-left pull-right'></i>\n";
            echo "          </span>\n";
            echo "        </a>\n";
            echo "        <ul class='treeview-menu'>\n";
            echo "          <li ><a href='javascript:void(0)' onclick='show_bodyinfo("; echo $project_id; echo ");return false;'><i class='fa fa-heartbeat'></i> <span>"; echo $left_side_bar_bodyinfo; echo "</span></a></li>\n";
            echo "          <li ><a href='javascript:void(0)' onclick='show_bodyinfo_record("; echo $project_id; echo ");return false;'><i class='fa fa-heartbeat'></i> <span>"; echo $left_side_bar_bodyinfo_record; echo "</span></a></li>\n";
            echo "          <li ><a href='javascript:void(0)' onclick='show_bodyinfo_manual_input("; echo $project_id; echo ");return false;'><i class='fa fa-pencil-square-o'></i> <span>"; echo $left_side_bar_bodyinfo_manual_input; echo "</span></a></li>\n";
            if ($GL_CONFIG_ENABLE_FORACARE == 1)
            {
                echo "    <li ><a href='javascript:void(0)' onclick='show_foracare("; echo $project_id; echo ");return false;'><i class='fa fa-heartbeat'></i><span>"; echo $left_side_bar_foracare_bodyinfo; echo "</span></a></li>\n";
            }
            echo "        </ul>\n";
            echo "      </li>\n";
        }
    ?>

    <?php
        if ($GL_CONFIG_ENABLE_PUSHNOTIFICATION == 1)
        {
          echo "<li class='treeview "; if($pushnotification_active) echo 'active'; echo "' id='treeview_pushnotification'>\n";
          echo "<a href='#'>\n";
          echo "<i class='fa fa-commenting'></i> <span>"; echo $left_side_bar_pushnotification; echo "</span>\n";
          echo "<span class='pull-right-container'>\n";
          echo "    <i class='fa fa-angle-left pull-right'></i>\n";
          echo "</span>\n";
          echo "</a>\n";
          echo "<ul class='treeview-menu'>\n";
          echo "  <li ><a href='javascript:void(0)' onclick='show_pushnotification("; echo $project_id ; echo ");return false;'><i class='fa fa-commenting-o'></i><span>"; echo $left_side_bar_pushnotification; echo "</span></a></li>\n";
          echo " <li ><a href='javascript:void(0)' onclick='show_pushnotification_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $vilspushnotification_records; echo "</span></a></li>\n";
          //echo " <li ><a href='javascript:void(0)' onclick='show_survey("; echo $project_id ; echo ");return false;'><i class='fa fa-pencil-square-o'></i><span>"; echo $left_side_bar_survey; echo "</span></a></li>\n";
          //echo "  <li ><a href='javascript:void(0)' onclick='show_survey_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $left_side_bar_survey_records; echo "</span></a></li>\n";
          echo "</ul>\n";
          echo "</li>\n";
        }
      ?>

      <?php
        if ($GL_CONFIG_ENABLE_LOCATION == 1)
        {
          echo "<li class='treeview ";  if($location_active) echo 'active'; echo "' id='treeview_location'>\n";
          echo "    <a href='javascript:void(0)' onclick='show_location("; echo $project_id; echo ");return false;'><i class='fa fa-location-arrow'></i> <span>"; echo $left_side_bar_devices_location; echo "</span></a>\n";
          echo "</li>\n";
        }
       ?>

     <?php
       if ($GL_CONFIG_ENABLE_RANGING == 1)
       {
          echo " <li class='treeview "; if($ranging_active) echo 'active'; echo "' id='treeview_ranging'>\n";
          echo "    <a href='javascript:void(0)' onclick='show_ranging("; echo $project_id; echo ");return false;'><i class='fa fa-exchange'></i> <span>"; echo $left_side_bar_devices_ranging; echo "</span></a>\n";
          echo " </li>\n";
       }
     ?>

      <?php
        if ($GL_CONFIG_ENABLE_2DMAP == 1)
        {
          echo " <li class='treeview "; if($vils2dmap_active) echo 'active'; echo "' id='treeview_2dmap'>\n";
          echo "     <a href='javascript:void(0)' onclick='show_2dmap("; echo $project_id; echo ");return false;'><i class='fa fa-map-o'></i> <span>"; echo $left_side_bar_devices_2d; echo "</span></a>\n";
          echo " </li>\n";
       }
      ?>

     <?php
       if ($GL_CONFIG_ENABLE_3DMAP == 1)
       {
          echo " <li class='treeview "; if($vils3dmap_active) echo 'active';  echo "' id='treeview_3dmap'>\n";
          echo "     <a href='javascript:void(0)' onclick='show_3dmap("; echo $project_id; echo ");return false;'><i class='fa fa-cube'></i> <span>"; echo $left_side_bar_devices_3d; echo "</span></a>\n";
          echo " </li>\n";
       }
     ?>

     <?php
       if ($GL_CONFIG_ENABLE_FIELDCONTROLL == 1)
       {
          echo " <li class='treeview "; if($vilsfieldcontrol_active) echo 'active';  echo "' id='treeview_fieldcontrol'>\n";
          echo "     <a href='javascript:void(0)' onclick='show_fieldcontrol("; echo $project_id; echo ");return false;'><i class='fa fa-television'></i> <span>"; echo $left_side_bar_devices_fieldcontrol; echo "</span></a>\n";
          echo " </li>\n";
       }
     ?>

      <?php
        if ($GL_CONFIG_ENABLE_FIREFIGHTING == 1)
        {
            echo "<li class='treeview "; if($vilsfirefighting_active) echo 'active'; echo "' id='treeview_firefighting'>\n";
            echo "<a href='#'>\n";
            echo "<i class='fa fa-fire-extinguisher'></i> <span>"; echo $left_side_bar_devices_firefighting; echo "</span>\n";
            echo "<span class='pull-right-container'>\n";
            echo "    <i class='fa fa-angle-left pull-right'></i>\n";
            echo "</span>\n";
            echo "</a>\n";
            echo "<ul class='treeview-menu'>\n";
            echo "  <li ><a href='javascript:void(0)' onclick='show_firefighting_create("; echo $project_id ; echo ");return false;'><i class='fa fa-fire'></i><span>"; echo $firefighting_create; echo "</span></a></li>\n";
            echo " <li ><a href='javascript:void(0)' onclick='show_firefighting_record("; echo $project_id ; echo ");return false;'><i class='fa fa-pencil-square-o'></i><span>"; echo $firefighting_record; echo "</span></a></li>\n";
            echo " <li ><a href='javascript:void(0)' onclick='show_firefighting_history("; echo $project_id ; echo ");return false;'><i class='fa fa-television'></i><span>"; echo $firefighting_history; echo "</span></a></li>\n";
            echo "  <li ><a href='javascript:void(0)' onclick='show_firefighting_report("; echo $project_id ; echo ");return false;'><i class='fa fa-folder-open-o'></i><span>"; echo $firefighting_report; echo "</span></a></li>\n";
            echo "</ul>\n";
            echo "</li>\n";
        }
      ?>

      <?php
      if ($GL_CONFIG_ENABLE_VERSION_HISTORY == 1)
      {
          echo " <li class='treeview "; if($versionhistory_active) echo 'active'; echo "' id='treeview_versionhistory'>\n";
          echo "     <a href='javascript:void(0)' onclick='show_versionhistory("; echo $project_id; echo ");return false;'><i class='fa fa-wrench'></i> <span>"; echo $left_side_bar_devices_versionhistory; echo "</span></a>\n";
          echo " </li>\n";
      }
      ?>

     <?php
       if ($GL_CONFIG_ENABLE_ACTIVITY == 1)
       {
          echo " <li class='treeview "; if($activity_active) echo 'active'; echo "' id='treeview_activity'>\n";
          echo "     <a href='javascript:void(0)' onclick='show_activity("; echo $project_id; echo ");return false;'><i class='fa fa-chain'></i><span>"; echo $left_side_bar_devices_activity; echo "</span></a>\n";
          echo " </li>\n";
        }
    ?>

    <?php
    if ($GL_CONFIG_ENABLE_MAINTENANCE == 1)
    {
        echo " <li class='treeview "; if($maintenance_active) echo 'active'; echo "' id='treeview_maintenance'>\n";
        echo "     <a href='javascript:void(0)' onclick='show_maintenance("; echo $project_id; echo ");return false;'><i class='fa fa-wrench'></i> <span>"; echo $left_side_bar_devices_maintenance; echo "</span></a>\n";
        echo " </li>\n";
    }
    ?>

    <?php
    if ($GL_CONFIG_ENABLE_SURVEY == 1 || $GL_CONFIG_ENABLE_ASSIST == 1 )
    {
        echo "<li class='header'>"; echo $left_side_bar_workassign; echo"</li>\n";
        echo "<li class='treeview "; if($workassign_active) echo 'active'; echo "' id='treeview_workassign'>\n";
        echo "<a href='#'>\n";
        echo "<i class='fa fa-list'></i> <span>"; echo $left_side_bar_workassign; echo "</span>\n";
        echo "<span class='pull-right-container'>\n";
        echo "    <i class='fa fa-angle-left pull-right'></i>\n";
        echo "</span>\n";
        echo "</a>\n";
        echo "<ul class='treeview-menu'>\n";
        if ($GL_CONFIG_ENABLE_SURVEY == 1)
        {
            echo " <li ><a href='javascript:void(0)' onclick='show_survey("; echo $project_id ; echo ");return false;'><i class='fa fa-pencil-square-o'></i><span>"; echo $left_side_bar_survey; echo "</span></a></li>\n";
            echo " <li ><a href='javascript:void(0)' onclick='show_survey_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $left_side_bar_survey_records; echo "</span></a></li>\n";
        }
        if ($GL_CONFIG_ENABLE_ASSIST == 1)
        {
            echo " <li ><a href='javascript:void(0)' onclick='show_assist("; echo $project_id ; echo ");return false;'><i class='fa fa-pencil-square-o'></i><span>"; echo $left_side_bar_assist; echo "</span></a></li>\n";
            echo " <li ><a href='javascript:void(0)' onclick='show_assist_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $left_side_bar_assist_records; echo "</span></a></li>\n";
        }
        echo "</ul>\n";
        echo "</li>\n";
    }
    ?>

    <?php
    if ($GL_CONFIG_ENABLE_BILLBOARD == 1)
    {
        echo "<li class='header'>"; echo $left_side_bar_billboard; echo"</li>\n";
        echo "<li class='treeview "; if($billboard_active) echo 'active'; echo "' id='treeview_billboard'>\n";
        echo "<a href='#'>\n";
        echo "<i class='fa fa-list'></i> <span>"; echo $left_side_bar_billboard; echo "</span>\n";
        echo "<span class='pull-right-container'>\n";
        echo "    <i class='fa fa-angle-left pull-right'></i>\n";
        echo "</span>\n";
        echo "</a>\n";
        echo "<ul class='treeview-menu'>\n";
        echo " <li ><a href='javascript:void(0)' onclick='show_billboard("; echo $project_id ; echo ");return false;'><i class='fa fa-pencil-square-o'></i><span>"; echo $left_side_bar_billboard_create; echo "</span></a></li>\n";
        echo " <li ><a href='javascript:void(0)' onclick='show_billboard_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $left_side_bar_billboard_records; echo "</span></a></li>\n";
        echo "</ul>\n";
        echo "</li>\n";
    }
    ?>

    <!-- Curriculum-->
    <?php
        if($GL_CONFIG_ENABLE_CURRICULUM == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_curriculum; echo"</li>\n";
            echo " <li class='treeview "; if($curriculum_active) echo 'active'; echo"' id='treeview_curriculum'>\n";
            echo " <a href='#'>\n";
            echo " <i class='fa fa-list'></i> <span>"; echo $left_side_bar_curriculum; echo "</span>\n";
            echo " <span class='pull-right-container'>\n";
            echo "    <i class='fa fa-angle-left pull-right'></i>\n";
            echo " </span>\n";
            echo " </a>\n";
            echo " <ul class='treeview-menu'>\n";
            if ( $_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 6 )
            {
                echo "   <li ><a href='javascript:void(0)' onclick='show_curriculum("; echo $project_id; echo");return false;'><i class='fa fa-calendar'></i><span>"; echo $left_side_bar_curriculum_list; echo"</span></a>\n";
            }
            echo "   <li ><a href='javascript:void(0)' onclick='show_curriculum_records("; echo $project_id ; echo ");return false;'><i class='fa fa-list'></i><span>"; echo $left_side_bar_curriculum_records; echo"</span></a></li>\n";
            echo " </ul>\n";
            echo " </li>\n";
        }
    ?>
    <!-- /.Curriculum -->

    <!-- Courses-->
    <?php
        if($GL_CONFIG_ENABLE_COURSE == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_courses; echo"</li>\n";
            echo " <li class='treeview "; if($courses_active) echo 'active'; echo"' id='treeview_courses'>\n";
            echo "     <a href='javascript:void(0)' onclick='show_courses("; echo $project_id; echo");return false;'><i class='fa fa-calendar'></i> <span>"; echo $left_side_bar_courses_list; echo"</span></a>\n";
            echo " </li>\n";
        }
    ?>
    <!-- /.Courses -->


    <!-- Week Schedule -->
    <?php
        if($GL_CONFIG_ENABLE_WEEK_SCHEDULE == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_weekschedule; echo"</li>\n";
            echo " <li class='treeview "; if($weekschedule_active) echo 'active'; echo"' id='treeview_weekschedule'>\n";
            echo "     <a href='javascript:void(0)' onclick='show_weekschedule("; echo $project_id; echo");return false;'><i class='fa fa-calendar'></i> <span>"; echo $left_side_bar_weekschedule_list; echo"</span></a>\n";
            echo " </li>\n";
        }
    ?>
    <!-- /.Week Schedule -->

    <!-- Application Form -->
    <?php
        if($GL_CONFIG_ENABLE_APPLICATION_FORM == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_applicationform; echo"</li>\n";
            echo " <li class='treeview "; if($applicationform_active) echo 'active'; echo"' id='treeview_applicationform'>\n";
            echo "     <a href='javascript:void(0)' onclick='show_applicationform_leave("; echo $project_id; echo");return false;'><i class='fa fa-calendar'></i> <span>"; echo $left_side_bar_applicationform_leave; echo"</span></a>\n";
            echo " </li>\n";
        }
    ?>
    <!-- /.Application Form -->



    <!-- Footprint-->
    <?php
        if ($GL_CONFIG_ENABLE_FOOTPRINT == 1)
        {
            echo "    <li class='header'>"; echo $left_side_bar_footprint; echo"</li>\n";
            if($footprint_active)
                echo "    <li class='treeview active'  id='treeview_footprint'>";
            else
                echo "    <li class='treeview       '  id='treeview_footprint'>";
            // echo "        <a href='javascript:void(0)' onclick='show_footprint(".$project_id.");return false;'><i class='fa fa-paw'></i> <span>". $left_side_bar_footprint_list."</span></a>";

            echo "    <a href='#'>";
            echo "      <i class='fa fa-paw'></i> <span>" . $left_side_bar_footprint_list ."</span>";
            echo "      <span class='pull-right-container'>";
            echo "        <i class='fa fa-angle-left pull-right'></i>";
            echo "      </span>";
            echo "    </a>";
            echo "    <ul class='treeview-menu'>";
            if ($_SESSION['type'] == 2)
            {
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_tags(". $project_id . ");return false;'><i class='fa fa-paw'></i><span>" . $left_side_bar_footprint_tags ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_Areas(". $project_id . ");return false;'><i class='fa fa-location-arrow'></i><span>" . $left_side_bar_footprint_areas ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_GPS(". $project_id . ");return false;'><i class='fa fa-map-marker'></i><span>" . $left_side_bar_footprint_GPS ."</span></a></li>";
            }
            else
            {
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_tags(". $project_id . ");return false;'><i class='fa fa-paw'></i><span>" . $left_side_bar_footprint_tags ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_Courses(". $project_id . ");return false;'><i class='fa fa-calendar'></i><span>" . $left_side_bar_footprint_courses ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_Areas(". $project_id . ");return false;'><i class='fa fa-location-arrow'></i><span>" . $left_side_bar_footprint_areas ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_InOutArea(". $project_id . ");return false;'><i class='fa fa-location-arrow'></i><span>" . $left_side_bar_footprint_inoutarea ."</span></a></li>";
                echo "      <li ><a href='javascript:void(0)' onclick='show_footprint_GPS(". $project_id . ");return false;'><i class='fa fa-map-marker'></i><span>" . $left_side_bar_footprint_GPS ."</span></a></li>";
                // echo "      <li ><a href='javascript:void(0)' onclick='show_footprint(". $project_id . ");return false;'><i class='fa fa-paw'></i> <span>". $left_side_bar_footprint_history."</span></a></li>";
            }
            echo "    </ul>";
            echo "</li>";
        }
        // else
        // {
        //     echo "<li class='treeview       '  id='treeview_footprint'>";
        //     echo "    <a  class='disabled'>";
        //     echo "      <i class='fa fa-paw'></i> <span>" . $left_side_bar_footprint_list ."</span>";
        //     echo "    </a>";
        //     echo "</li>";
        // }
      ?>
      <!-- /.Footprint -->

      <!-- Events -->
      <?php
          if ($GL_CONFIG_ENABLE_EVENT == 1)
          {
              echo "<li class='header'>"; echo $left_side_bar_event; echo "</li>";
              echo "<li class='treeview "; if($events_active) echo 'active'; echo "' id='treeview_events'>";
              echo "      <a href='javascript:void(0)' onclick='show_events("; echo $project_id; echo ");return false;'><i class='fa fa-bell'></i> <span>"; echo $left_side_bar_event_llist; echo "</span></a>";
              echo "</li>";
          }
      ?>
      <!-- /.Events -->

      <!-- inspection -->
      <?php
        if ($GL_CONFIG_ENABLE_INSPECTION == 1)
        {
            echo "    <li class='header'>"; echo $left_side_bar_setting_inspection_tree; echo"</li>\n";
            if($inspection_active)
                echo "<li class='treeview active'  id='treeview_inspection'>";
            else
                echo "<li class='treeview       '  id='treeview_inspection'>";
            echo "    <a href='#'>";
            echo "      <i class='fa fa-eye'></i> <span>"; echo $left_side_bar_setting_inspection; echo"</span>";
            echo "      <span class='pull-right-container'>";
            echo "        <i class='fa fa-angle-left pull-right'></i>";
            echo "      </span>";
            echo "    </a>";
            echo "    <ul class='treeview-menu'>";
            echo "      <li ><a href='javascript:void(0)' onclick='show_inspection_report("; echo $project_id; echo");return false;'><i class='fa fa-newspaper-o'></i><span>"; echo $left_side_bar_setting_inspection_report ; echo"</span></a></li>";
            echo "      <li ><a href='javascript:void(0)' onclick='show_inspection_setting("; echo $project_id; echo");return false;'><i class='fa fa-cog'></i><span>"; echo $left_side_bar_setting_inspection_setting ; echo"</span></a></li>";
            echo "      <li ><a href='javascript:void(0)' onclick='show_inspection_record("; echo $project_id; echo");return false;'><i class='fa fa-file-text-o'></i><span>"; echo $left_side_bar_setting_inspection_dataview ; echo"</span></a></li>";
            echo "      <li ><a href='javascript:void(0)' onclick='show_inspection_history("; echo $project_id; echo");return false;'><i class='fa fa-clipboard'></i><span>"; echo $left_side_bar_setting_inspection_history ; echo"</span></a></li>";
            echo "    </ul>";
            echo "</li>";
        }
        // else
        // {
        //     echo "<li class='treeview       '  id='treeview_inspection'>";
        //     echo "    <a  class='disabled'>";
        //     echo "      <i class='fa fa-eye'></i> <span>" . $left_side_bar_setting_inspection ."</span>";
        //     echo "    </a>";
        //     echo "</li>";
        // }
      ?>

      <?php
        if($GL_CONFIG_ENABLE_SECTION == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_setting_section_tree; echo "</li>\n";
            echo " <li class='treeview "; if($sections_active) echo 'active'; echo "' id='treeview_sections'>";
            echo "     <a href='javascript:void(0)' onclick='show_sections("; echo $project_id; echo");return false;'><i class='fa fa-object-ungroup'></i> <span>"; echo $left_side_bar_setting_section; echo "</span></a>";
            echo " </li>";
        }
        ?>

      <?php
        if ($GL_CONFIG_ENABLE_FENCE == 1)
        {
            echo " <li class='header'>"; echo $left_side_bar_setting_fencealert_tree; echo "</li>\n";
            if($fencealert_active)
                echo "<li class='treeview active'  id='treeview_fencealert'>";
            else
                echo "<li class='treeview       '  id='treeview_fencealert'>";

          echo "<a href='javascript:void(0)' onclick='show_fencealert(". $project_id.");return false;'><i class='fa fa-warning'></i> <span>".$left_side_bar_setting_fencealert."</span></a>";
          echo "</li>";
        }
        // else
        // {
        //     echo "<li class='treeview       '  id='treeview_fencealert'>";
        //     echo "    <a  class='disabled'>";
        //     echo "      <i class='fa fa-warning'></i> <span>" . $left_side_bar_setting_fencealert ."</span>";
        //     echo "    </a>";
        //     echo "</li>";
        // }
      ?>

      <?php
        if ($GL_CONFIG_ENABLE_FILESTORAGE == 1)
        {
            echo "<li class='header'>"; echo $left_side_bar_filestorage; echo "</li>";
            echo " <li class='treeview "; if($filestorage_active) echo 'active'; echo "' id='treeview_filestorage'>\n";
            echo "    <a href='#'>\n";
            echo "       <i class='fa fa-folder-open'></i> <span>". $left_side_bar_filestorage ."</span>";
            echo "          <span class='pull-right-container'>";
            echo "            <i class='fa fa-angle-left pull-right'></i>";
            echo "          </span>\n";
            echo "    </a>\n";
            echo "    <ul class='treeview-menu'>\n";
            echo "       <li ><a href='javascript:void(0)' onclick='show_filestorage_image(". $project_id .");return false;'><i class='fa fa-camera'></i> <span>". $left_side_bar_filestorage_image ."</span></a></li>";
            echo "       <li ><a href='javascript:void(0)' onclick='show_filestorage_file(". $project_id .");return false;'><i class='fa fa-paperclip'></i> <span>". $left_side_bar_filestorage_file ."</span></a></li>";
            echo "    </ul>\n";
            echo " </li>\n";
        }
      ?>

      <!-- MQTT Application -->
      <?php

        if ($GL_CONFIG_ENABLE_MQTTApplication == 1)
        {
            echo "<li class='header'>$left_side_bar_scotttest</li>";
            echo "  <li class='treeview "; if($scotttest_active) echo 'active'; echo "' id='treeview_scotttest'>\n";
            echo "    <a href='#'>\n";
            echo "      <i class='fa fa-cogs'></i> <span>$left_side_bar_scotttest</span>";
            echo "        <span class='pull-right-container'>";
            echo "          <i class='fa fa-angle-left pull-right'></i>";
            echo "        </span>\n";
            echo "      </i>\n";
            echo "    </a>\n";
            echo "    <ul class='treeview-menu'>\n";
            echo "      <li><a href='javascript:void(0)' onclick='show_eventReport($project_id);return false;'><i class='fa fa-camera'></i> <span>$left_side_bar_eventreport</span></a></li>";
            echo "      <li><a href='javascript:void(0)' onclick='show_powermeterview($project_id);return false;'><i class='fa fa-bolt'></i> <span>$left_side_bar_powermeterview</span></a></li>\n";
            echo "      <li><a href='javascript:void(0)' onclick='show_lightdevicecontrol($project_id);return false;'><i class='fa fa-bolt'></i> <span>$left_side_bar_lightdevicecontrol</span></a></li>\n";
            echo "      <li><a href='javascript:void(0)' onclick='show_watchlocation($project_id);return false;'><i class='fa fa-bolt'></i> <span>$left_side_bar_watchlocation</span></a></li>\n";
            echo "    </ul>\n";
            echo "  </li>\n";
        }

      ?>
      <!-- /.MQTT Application -->

      <!-- Setting -->
      <?php
        if ($GL_CONFIG_ENABLE_SETTING == 1)
        {
            echo "<li class='header'>"; echo $left_side_bar_setting; echo "</li>";

            if($settings_active)
                echo "<li class='treeview active'  id='treeview_settings'>";
            else
                echo "<li class='treeview       '  id='treeview_settings'>";

            echo "<a href='javascript:void(0)' onclick='show_settings(". $project_id.");return false;'><i class='fa fa-gear'></i> <span>".$left_side_bar_setting_system."</span></a>";
            echo "</li>";

            if ($GL_CONFIG_ENABLE_DI_SETTING == 1)
            {
                if($disetting_active)
                    echo "<li class='treeview active'  id='treeview_disetting'>";
                else
                    echo "<li class='treeview       '  id='treeview_disetting'>";

                echo "<a href='javascript:void(0)' onclick='show_disetting(". $project_id.");return false;'><i class='fa fa-th-large'></i> <span>".$left_side_bar_setting_di."</span></a>";
                echo "</li>";
            }
        }
        // else
        // {
        //     echo "<li class='treeview       '  id='treeview_settings'>";
        //     echo "    <a  class='disabled'>";
        //     echo "      <i class='fa fa-eye'></i> <span>" . $left_side_bar_setting_system ."</span>";
        //     echo "    </a>";
        //     echo "</li>";
        // }

        if ($GL_CONFIG_ENABLE_SETTING == 1 && $GL_CONFIG_ENABLE_PUSHNOTIFICATION == 1)
        {
            if($pushnotify_active)
                echo "<li class='treeview active'  id='treeview_pushnotify'>";
            else
                echo "<li class='treeview       '  id='treeview_pushnotify'>";

          echo "<a href='javascript:void(0)' onclick='show_pushnotify(". $project_id.");return false;'><i class='fa fa-comment-o'></i> <span>".$left_side_bar_setting_pushnotify."</span></a>";
          echo "</li>";
        }
      ?>


      <!-- /.Setting -->

      <!-- Engineering Mode -->
      <?php
        if ($GL_CONFIG_ENABLE_ENGINEERMODE)
        {
              echo "<li class='header'>". $left_side_bar_EM ."</li>";
              if($checkpoint_active)
                echo "<li class='treeview active' id='treeview_diagnosis'>";
              else
                echo "<li class='treeview       ' id='treeview_diagnosis'>";
              echo "    <a href='javascript:void(0)' onclick='show_diagnosis(".$project_id .");return false;'><i class='fa fa-cogs'></i> <span> ".$left_side_bar_EM_Diagnosis ."</span></a>";
              echo "</li>";
              if($sections_active)
                echo "<li class='treeview active' id='treeview_analyzer'>";
              else
                echo "<li class='treeview       ' id='treeview_analyzer'>";
              echo "    <a href='javascript:void(0)' onclick='show_analyzer( ".$project_id .");return false;'><i class='fa fa-cogs'></i> <span> ".$left_side_bar_EM_Analyzer ."</span></a>";
              echo "</li>";
        }
      ?>
      <!-- /.Engineering Mode -->



    </ul>
    <!-- /.sidebar-menu -->
  </section>
  <!-- /.sidebar -->
</aside>
