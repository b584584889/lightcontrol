<?php
    require_once("SMIMS_config.php");

    $ALERT_BOX_FONT_SIZE = 8;

    $login_tilte = '北瀚科技 室內定位系統';
    $login_sign_in_comment = '請登入帳號';
    $login_sign_in_account = '帳號';
    $login_sign_in_password = '密碼';
    $login_sign_in_loginbtn = '登入';

    $page_tilte = '友達頤康 IPS Solutions';
    $main_show_logo = true;
    $main_logo_file = '/images/auo_logo.jpg';
    $main_bottom_title = '友達頤康 IPS solutions';
    $main_copyright_name = '&nbsp;';
    $main_copyright = '&nbsp;';

    $main_lang_seeall = '看全部';

    $left_side_bar_dashboard_list = '系統概覽';
    $left_side_bar_devices = '装置';
    $left_side_bar_devices_list = '裝置管理';
    $left_side_bar_userinfo = '人員管理';
    $left_side_bar_userlist = '人員列表';
    $left_side_bar_usergroupinfo = '權限群組';
    $left_side_bar_subgroupinfo = '子集合';
    $left_side_bar_bodyinfo = '生理量測';
    $left_side_bar_bodyinfo_record = '生理量測紀錄';
    $left_side_bar_foracare_bodyinfo = '第三方裝置即時量測資訊';
    $left_side_bar_pushnotification = '推播訊息';
    $left_side_bar_survey = '待辦事項';
    $left_side_bar_survey_records = '待辦事項紀錄';
    $left_side_bar_devices_location = '区域';
    $left_side_bar_devices_ranging = '测距结果';
    $left_side_bar_devices_2d = '2D 地图';
    $left_side_bar_devices_3d = '3D 地图';
    $left_side_bar_devices_activity = '活动状态';
    $left_side_bar_devices_maintenance = '裝置履歷';

    $left_side_bar_courses = '课程';
    $left_side_bar_courses_list = '课程表';

    $left_side_bar_footprint = '足迹';
    $left_side_bar_footprint_tags = '标籤足迹';
    $left_side_bar_footprint_courses = '课程记录';
    $left_side_bar_footprint_areas = '区域记录';
    $left_side_bar_footprint_inoutarea = '进出区域';
    $left_side_bar_footprint_GPS = 'GPS足迹';
    $left_side_bar_footprint_list = '足迹记录';
    $left_side_bar_footprint_history = '历史足迹';

    $left_side_bar_event = '事件';
    $left_side_bar_event_llist = '事件列表';

    $left_side_bar_setting = '設定';
    // $left_side_bar_setting_checkpoint = '巡檢路線';
    // $left_side_bar_setting_checkpoint_setting = '巡檢路線设定';
    // $left_side_bar_setting_checkpoint_dataview = '每日记录';
    // $left_side_bar_setting_checkpoint_history = '歷史记录';
    $left_side_bar_setting_inspection = '巡檢路線';
    $left_side_bar_setting_inspection_report = '巡檢報告';
    $left_side_bar_setting_inspection_setting = '巡檢路線设定';
    $left_side_bar_setting_inspection_dataview = '每日记录';
    $left_side_bar_setting_inspection_history = '歷史记录';
    $left_side_bar_setting_section = '区段设定';
    $left_side_bar_setting_fencealert = '電子圍籬';
    $left_side_bar_setting_system = '系统设定';
    $left_side_bar_setting_pushnotify = '推播項目設定';

    $left_side_bar_EM = 'Engineering Mode';
    $left_side_bar_EM_Diagnosis = 'Diagnosis';
    $left_side_bar_EM_Analyzer = 'Analyzer';

    $vilsdevices_devicelist = '装置列表';
    $vilsdevices_tag = '标籤';
    $vilsdevices_anchor = '基站';
    $vilsdevices_coordinator = '闸道';
    $vilsdevices_locator = 'Locator';
    $vilsdevices_showfilter = '顯示項目';
    $vilsdevices_filter_showonline = '在線裝置';
    $vilsdevices_filter_showoffline = '離線裝置';

    $vilsdevices_loragateway = 'LoRa Gateway';

    $vilsdevices_alive = '活动';
    $vilsdevices_ping = '网路';
    $vilsdevices_MQTT = 'MQTT';
    $vilsdevices_deviceid = '装置序号';
    $vilsdevices_devicename = '装置名称';
    $vilsdevices_typeindex = '形式/编号';
    $vilsdevices_role = '角色';
    $vilsdevices_buttontype = '按鈕型態';
    $vilsdevices_resident = '住戶';
    $vilsdevices_user = '人員';
    $vilsdevices_macaddress = '硬体位址';
    $vilsdevices_position = '座标';
    $vilsdevices_sensoralert = '入侵警示';
    $vilsdevices_GPS = 'GPS';
    $vilsdevices_maplayer = '圖層';
    $vilsdevices_building = '楼号';
    $vilsdevices_floor = '楼层';
    $vilsdevices_info = '资讯';
    $vilsdevices_available_anchors = '可用基站';
    $vilsdevices_projects = '可用專案';
    $vilsdevices_IP = 'IP';
    $vilsdevices_version = '版本';
    $vilsdevices_PID = 'PID';
    $vilsdevices_CoordRunTime = '運行時間';
    $vilsdevices_blacklist = '黑名單';
    $vilsdevices_whitelist = '白名單';
    $vilsdevices_joinRangingRange = 'Anchor可Join且可測距範圍';
    $vilsdevices_joinNotRangingRange = 'Anchor可Join但不可測距範圍';
    $vilsdevices_reset = '重置';
    $vilsdevices_LoRA = 'LoRA位址';
    $vilsdevices_general = '一般设定';
    $vilsdevices_editinfo = '编辑装置资讯';
    $vilsdevices_toggle = '切换';
    $vilsdevices_loading = '载入中...';
    $vilsdevices_gwid = 'gwid';
    $vilsdevices_sf = 'sf';
    $vilsdevices_repeater = 'repeater';
    $vilsdevices_channel = 'channel';

    $vilspushnotification_title = '推播訊息';
    $vilspushnotification_sendto = '推播訊息給';
    $vilspushnotification_tag = '標籤';
    $vilspushnotification_locator = 'Locator';
    $vilspushnotification_role = '角色';
    $vilspushnotification_records = '推播紀錄';

    $vilssurvey_title = '待辦事項';
    $vilssurvey_createnewsurvey = '新增待辦事項';

    $vilssurveyrecord_title = '待辦事項紀錄';

    $vilsuserinfo_title = '人員列表';
    $vilsuserinfo_name = '名稱';
    $vilsuserinfo_account = '帳號';
    $vilsuserinfo_unit = '單位';
    $vilsuserinfo_usertitle = '職稱';
    $vilsuserinfo_gender = '性別';
    $vilsuserinfo_group = '權限群組';
    $vilsuserinfo_creatnewuser = '新增人員';

    $vilsusergroupinfo_title = '群組列表';
    $vilsusergroupinfo_groupname = '名稱';
    $vilsusergroupinfo_permission = '權限';
    $vilsusergroupinfo_operation = '操作';
    $vilsusergroupinfo_creatnewusergroup = '新增群組';

    $vilsbodyinfo_title = '生理量測';
    $vilsbodyinfo_abbox = '訊號採集器';

    $vilslocation_title = '标籤所在区域';
    $vilslocation_location = '区域';
    $vilslocation_type = '型態';
    $vilslocation_activity = '活动';
    $vilslocation_device = '装置序号/名称';

    $vilsranging_title = '测距记录';
    $vilsranging_loading = '载入中...';

    $vils2dmap_title = '室内地图';
    $vils2dmap_edit_area_name = '编辑区域名称';
    $vils2dmap_edit_area_type_0 = '普通';
    $vils2dmap_edit_area_type_1 = '電子圍欄';
    $vils2dmap_edit_area_type_2 = '電子點名';
    $vils2dmap_edit_area_type_3 = '房間';
    $vils2dmap_edit_area_type_4 = '教室';
    $vils2dmap_edit_area_type_5 = '病房';
    $vils2dmap_edit_area_type_6 = '病床';
    $vils2dmap_edit_area_type_999 = '虛擬牆';
    $vils2dmap_edit_save = '储存';
    $vils2dmap_general = '一般设定';
    $vils2dmap_changemap = '更换地图';
    $vils2dmap_show_locators = '顯示固定裝置';
    $vils2dmap_show_tags = '顯示标籤';

    $vilsactivity_title = '活动状态';
    $vilsactivity_tag = '标籤';
    $vilsactivity_anchor = '基站';
    $vilsactivity_coordinator = '闸道';
    $vilsactivity_locator = 'Locator';
    $vilsactivity_typeindex = '形式/编号';
    $vilsactivity_macaddress = '硬体位址';
    $vilsactivity_timestamp = '时间';
    $vilsactivity_activity = '活动';

    $vilsmaintenance_title = '裝置履歷';
    $vilsmaintenance_nodetype = '裝置形式';
    $vilsmaintenance_nodeid = '裝置序號';
    $vilsmaintenance_name = '儀器名稱';
    $vilsmaintenance_type = '廠牌型別';
    $vilsmaintenance_unit = '保管單位';
    $vilsmaintenance_manager = '管理人員';
    $vilsmaintenance_assetlife = '資產年限';
    $vilsmaintenance_maintenancedate = '保養校正(日期通知)';
    $vilsmaintenance_status = '儀器設備狀況(良好、維修校正、故障)';
    $vilsmaintenance_datetime = '紀錄時間';
    $vilsmaintenance_createnewrecord = '新增紀錄';

    $vilsfootprint_title = '足迹记录';
    $vilsfootprint_select_tag = '选择标籤';
    $vilsfootprint_pause = '暂停';
    $vilsfootprint_play = '播放';
    $vilsfootprint_speed_1 = '1倍速度';
    $vilsfootprint_speed_4 = '4倍速度';
    $vilsfootprint_speed_16 = '16倍速度';
    $vilsfootprint_speed_60 = '60倍速度';

    $vilsfootprint_courses_list = '課程列表';
    $vilsfootprint_courses_name = '課程名稱';
    $vilsfootprint_courses_description = '課程描述';
    $vilsfootprint_courses_datetime = '日期時間';
    $vilsfootprint_areas_name = '區域名稱';
    $vilsfootprint_areas_type = '區域型態';

    $vilsfootprint_reload = '繼續載入';
    $vilsfootprint_nodata = '已無資料';

    $vilsevent_title = '标籤触发事件';

    $vilssection_title = '区段设定';
    $vilssection_Anchor = '使用基站';
    $vilssection_Anchor_1 = '做判断, ';
    $vilssection_At = '所在区域';
    $vilssection_Range = '范围内(公分):';
    $vilssection_start_at = '座标开始点(X,Y,Z)';
    $vilssection_to_end = '到结束点(X,Y,Z)';
    $vilssection_create = '新增设定';

    $vilsfencealert_title = '電子圍籬';
    $vilsfencealert_Anchor = '使用基站';
    $vilsfencealert_Anchor_1 = '做判斷, ';
    $vilsfencealert_At = '所在區域';
    $vilsfencealert_Range = '範圍內(公分):';
    $vilsfencealert_create = '新增設定';

    $vilssetting_title = '专案设定';
    $vilssetting_building = '楼号';
    $vilssetting_floor = '楼层';
    $vilssetting_restart = '重新启动';
    $vilssetting_save = '储存';

    $vilspushnotify_title = '推播項目設定';
    $vilspushnotify_target = '設定對象';
    $vilspushnotify_employees = '工作人員';
    $vilspushnotify_setting = '推播設定';
    $vilspushnotify_search = '搜尋';

    // $vilscheckpointpath_title = '巡檢路線';
    // $vilscheckpointpath_setting_title = '巡檢路線設定';
    // $vilscheckpointpath_record_title = '巡檢路線記錄';
    // $vilscheckpointpath_loading = '载入中...';
    // $vilscheckpointrecord_loading  = '载入中...';
    //
    // $vilscheckpointhistory_title = '巡檢路線歷史记录';

    $vilsinspectionpath_title = '巡檢路線';
    $vilsinspectionpath_setting_title = '巡檢路線設定';
    $vilsinspectionpath_record_title = '巡檢路線記錄';
    $vilsinspectionpath_loading = '載入中...';
    $vilsinspectionrecord_loading  = '载入中...';

    $vilsinspectionhistory_title = '巡檢路線歷史記錄';

    $vilsinspection_report_title = '巡檢報告';
    $vilsinspection_daily_report = '日報表';
    $vilsinspection_weekly_report = '週報表';
    $vilsinspection_monthly_report = '月報表';
    $vilsinspection_generate_report = '產生報表';

    $vilsalerteventtimeline_alive = '活动';
    $vilsalerteventtimeline_floor = '楼层';
    $vilsalerteventtimeline_timeline = '事件时间轴';

?>
