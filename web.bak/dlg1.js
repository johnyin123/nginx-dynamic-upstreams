<script type="text/javascript">  
	//如果href中有modal_form这个id的，那么点击后会调用如下函数  
	$("[href='#modal_form']").click(function(){  
		//定义modal_id，确保每个按钮所产生的id不一样，否则将会是最先遇到的那个  
		var modal_id = "#" + $(this).attr('id');  
		//填充modal的各种元素  
		$('.modal-title').text($(modal_id).attr('title'));  
		$('.modal-body').text($(modal_id).attr('content'));  
		//form提交  
		$('#myForm').on('submit', function(e){  
			//防止冒泡  
			e.preventDefault();  
			//提交到相应的rel上  
			$.post($(modal_id).attr('rel'),  
				$(this).serialize(),  
				function(data, status, xhr) {  
					//网页返回正常且删除操作成功  
					if (status == 'success' && data == 'success') {  
						$('.modal-body').text('操作成功');  
						$('.modal-footer').html('<button type="button" class="btn btn-default" data-dismiss="modal">返回</button>');  
						//提交失败的操作  
					} else {  
						$('.modal-body').text('操作失败，请重新选择条目，或联系管理人员');  
						$('.modal-footer').html('<button type="button" class="btn btn-default" data-dismiss="modal">返回</button>');  
					}  
				});  
		});  
	});  
//当modal页面消失后，重新将页面刷新  
$(function () { $('#modal_form').on('hide.bs.modal', function () {  
	window.location.reload();})  
});  
</script>  
