var upx_mgr_url = "http://localhost:8800/ups";

var tpl_lst = '{{#ups}}<legend><h2>{{upsname}}({{type}})[{{number}}]<small><p class="text-right">Total_Weight({{total_weight}})</p></small></h2></legend><label><button class="btn btn-primary" onclick=add("{{upsname}}","{{type}}")>add</button><button class="btn btn-primary" onclick=del("{{upsname}}","{{type}}")>del</button></label><br><table name="{{upsname}}" class="table table-bordered"><thead><tr><th>name</th><th>server</th><th>w</th><th>mc</th><th>mf</th><th>f_tm</th><th>dn</th><th>bk</th><th>Operations</th></tr></thead><tbody>{{#peers}}<tr><td key=name t=s>{{name}}</td><td key=server t=s>{{server}}</td><td key=weight t=i>{{weight}}</td><td key=max_conns t=i>{{max_conns}}</td><td key=max_fails t=i>{{max_fails}}</td><td key=fail_timeout t=i>{{fail_timeout}}</td><td key=down t=b>{{down}}</td><td key=backup t=b>{{backup}}</td><td><a style="cursor:pointer;" onclick=mdf(this,"{{upsname}}","{{type}}")>edit</a></td></tr>{{/peers}}</tbody></table>{{/ups}}';

var dlg_add_tbl = '<table class="table table-condensed table-bordered" id="dlg_add_tbl"><thead><tr><th>#</th><th>server</th><th>w</th><th>mc</th><th>mf</th><th>ftm</th><th>bk</th><th>dn</th></tr><tbody></tbody></table>';
var dlg_del_tbl = '';
function set_dlg_title(dlg, title) {
	dlg.find('.modal-title').text(title);
}
function reset_ups_visiable_item() {
	$('#dlg_it_server').val('');
	$('#dlg_it_weight').val('10');
	$('#dlg_it_max_conns').val('0');
	$('#dlg_it_max_fails').val('0');
	$('#dlg_it_fail_timeout').val('30');
	$('#dlg_it_down').prop('checked', false);
	$('#dlg_it_backup').prop('checked', false);
}
function reset_ups_dlg() {
	$('#dlg_it_method').val('');
	$('#dlg_it_type').val('');
	$('#dlg_it_upsname').val('');
	$('#dlg_it_name').val('');
	reset_ups_visiable_item();
	$('.btn_dlg_add').hide();
	$('.btn_dlg_del').hide();
	$('#dlg_peers_info').html('');

	$('#ups_dlg_btn_del').click(function(){tbl_submit_dummuy();});
	$('#ups_dlg_btn_add').click(function(){tbl_submit_dummuy();});
	$('#ups_dlg_submit') .click(function(){tbl_submit_dummuy();});

}

function tbl_add()
{
	var server        =$('#dlg_it_server').val();
	var weight        =$('#dlg_it_weight').val();
	var max_conns     =$('#dlg_it_max_conns').val();
	var max_fails     =$('#dlg_it_max_fails').val();
	var fail_timeout  =$('#dlg_it_fail_timeout').val();
	var backup        =$('#dlg_it_down').prop('checked') ? 'true' : 'false';
	var down          =$('#dlg_it_backup').prop('checked') ? 'true' : 'false';

	var table1 = $('#dlg_add_tbl');
	var firstTr = table1.find('tbody>tr:first');
	var row = $('<tr></tr>');
	var td = $('<td></td>');
	td.append($('<input type="checkbox" name="tbl_ptr"/>'));
	row.append(td);
	td = $('<td key="server" t="s"></td>'); td.append(server); row.append(td);
	td = $('<td key="weight" t="i"></td>'); td.append(weight); row.append(td);
	td = $('<td key="max_conns" t="i"></td>'); td.append(max_conns); row.append(td);
	td = $('<td key="max_fails" t="i"></td>'); td.append(max_fails); row.append(td);
	td = $('<td key="fail_timeout" t="i"></td>'); td.append(fail_timeout); row.append(td);
	td = $('<td key="backup" t="b"></td>'); td.append(backup); row.append(td);
	td = $('<td key="down" t="b"></td>'); td.append(down); row.append(td);
	table1.append(row);
	reset_ups_visiable_item();
}
function tbl_del()
{
	var checked = $('input[type="checkbox"][name="tbl_ptr"]');
	$(checked).each(function(){
		if($(this).prop('checked'))
		{
			$(this).parent().parent().remove();
		}
	});
}
function tbl_submit_dummuy() {
}
function tbl_submit_add(){
	$('#ups_dlg_submit') .click(function(){tbl_submit_dummuy();});

	var checked = $('input[type="checkbox"][name="tbl_ptr"]');
	var arr = new Array();
	$(checked).each(function(){
		var jstr = getTableContent(this);
		jobj = JSON.parse(jstr);
		arr.push(jobj);
	});
	var type = $('#dlg_it_type').val();
	var upsname = $('#dlg_it_upsname').val();
	var params = new Array();
	if(type == 'http') {
		params.push({'http':{'upsname':upsname, 'peers':arr}});
	}
	else {
		params.push({'stream':{'upsname':upsname, 'peers':arr}});
	}
	var obj = { 'method': 'add', 'params' : params};

	$("#ups_dlg").modal("hide");

	$.ajax({
		//提交的网址
		url:upx_mgr_url,
		//提交的数据
		data: JSON.stringify(obj),
		//提交数据的类型 POST GET
		type:'post',
		cache:false,
		processData:false,
		//datatype: 'json',//'xml', 'html', 'script', 'json', 'jsonp', 'text'.
		datatype:'json',
		//在请求之前调用的函数
		beforeSend:function(){
			//$('#status').html('logining');
		},
		//成功返回之后调用的函数
		success:function(data, textStatus) {
			//$('#status').html(textStatus);
			if(data.code == 0) {
				alert('success :' + data.message.success +'\nfailed:'+ data.message.failed)
			} else {
				alert(data.message);
			}
		},
		error: function(XMLHttpRequest, textStatus, errorThrown) {
			$('status').html(textStatus);
		},
		complete:function(XMLHttpRequest, textStatus) {
			//$('#msg').html('complete');
		}
	});

}
function add(ups, type) {
	set_dlg_title($('#ups_dlg'), 'Add Upstream(' + ups +')');	
	reset_ups_dlg();
	$('.btn_dlg_add').show();
	$('.btn_dlg_del').show();
	$('#dlg_peers_info').html(dlg_add_tbl);
	$('#ups_dlg_btn_del').click(function(){tbl_del();});
	$('#ups_dlg_btn_add').click(function(){tbl_add();});
	$('#ups_dlg_submit').click(function(){tbl_submit_add();});
	$('#dlg_it_method').val('add');
	$('#dlg_it_type').val(type);
	$('#dlg_it_upsname').val(ups);
	$('#ups_dlg').modal('show');
}


function mdf(node, ups, type) {
	set_dlg_title($('#ups_dlg'), 'Edit Upstream(' + ups +')');	
	reset_ups_dlg();
	jstr = getTableContent(node);
	$('#dlg_it_method').val('mdf');
	$('#dlg_it_type').val(type);
	$('#dlg_it_upsname').val(ups);
	json = JSON.parse(jstr);
	$('#dlg_it_name').val(json.name);
	$('#dlg_it_server').val(json.server);
	$('#dlg_it_weight').val(json.weight);
	$('#dlg_it_max_conns').val(json.max_conns);
	$('#dlg_it_max_fails').val(json.max_fails);
	$('#dlg_it_fail_timeout').val(json.fail_timeout);
	$('#dlg_it_down').prop('checked', json.down);
	$('#dlg_it_backup').prop('checked', json.backup);
	$('#ups_dlg').modal('show');
}

function getTableContent(node) {
	var obj = {};
	//按钮的父节点的父节点是tr。
	var tr1 = node.parentNode.parentNode;
	for(var i=0;i<tr1.cells.length;i++) {
		key = tr1.cells[i].getAttribute('key');
		if(key != null) {
			type = tr1.cells[i].getAttribute('t');
			value = tr1.cells[i].innerHTML;
			if (type == 'i') {
				obj[key] = Number(value);
			}
			else if (type == 'b') {
				obj[key] = value === 'true';
			}
			else {
				obj[key] = value;
			}
		}
	}
	return JSON.stringify(obj);
}

function lst() {
	$.ajax({
		//提交的网址
		url:upx_mgr_url,
		//提交的数据
		data: JSON.stringify({ 'method': 'lst' }),
		//提交数据的类型 POST GET
		type:'post',
		cache:false,
		processData:false,
		//datatype: 'json',//'xml', 'html', 'script', 'json', 'jsonp', 'text'.
		datatype:'json',
		//在请求之前调用的函数
		beforeSend:function(){
			//$('#status').html('logining');
		},
		//成功返回之后调用的函数
		success:function(data, textStatus) {
			//$('#status').html(textStatus);
			show_tbl($('#peers_info'), data);
		},
		error: function(XMLHttpRequest, textStatus, errorThrown) {
			$('status').html(textStatus);
		},
		complete:function(XMLHttpRequest, textStatus) {
			//$('#msg').html('complete');
		}
	});
}

function show_tbl(divobj, data){
	if(data.code == 0) {
		//var str_obj = JSON.parse(data);
		var objs = { ups:data.message };
		var views = Mustache.render(tpl_lst, objs);
		divobj.html(views);
	} else {
		alert(data.message);
	}
}
$(document).ready(function(){
	$('#lst').click(function(){
	//$('.lst_btn').click(function(){
		lst();
	});
});
