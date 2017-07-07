		<div class='modal fade' id='editdlg' tabindex='-1' role='dialog' aria-labelledby='editdlglabel'>
			<div class='modal-dialog' role='document'>
				<div class='modal-content'>
					<div class='modal-header'>
						<button type='button' class='close' data-dismiss='modal' aria-label='Close'><span aria-hidden='true'>&times;</   span></button>
						<h4 class='modal-title' id='editdlglabel'>Edit</h4>
					</div>
					<div class='modal-body'>
						<form class='form-horizontal'>
							<div class='form-group'>
								<label for='m_server' class='col-sm-3 control-label'>Server:</label>
								<p class='col-sm-9 form-control-static' id='m_server'>192.168.1.105:8525</p>
							</div>
							<div class='form-group'>
								<label for='m_weight' class='col-sm-3 control-label'>Weight:</label>
								<div class='col-sm-9'><input type='text' class='form-control' id='m_weight' value=''/></div>
							</div>
							<div class='form-group'>
								<label for='m_backup' class='col-sm-3 control-label'>Backup:</label>
								<div class='col-sm-9'><select class='form-control' id='m_backup'>
										<option value='1'>Yes</option>
										<option value='0'>No</option>
									</select></div>
							</div>
							<div class='form-group'>
								<label for='m_max_fails' class='col-sm-3 control-label'>max_fails:</label>
								<div class='col-sm-9'><input type='text' class='form-control' id='m_max_fails' value=''/></div>
							</div>
							<div class='form-group'>
								<label for='m_fail_timeout' class='col-sm-3 control-label'>fail_timeout:</label>
								<div class='col-sm-9'><input type='text' class='form-control' id='m_fail_timeout' value=''/></div>
							</div>
						</form>
					</div>
					<div class='modal-footer'>
						<button type='button' class='btn btn-primary' id='edit_btn'>OK</button>
					</div>
				</div>
			</div>
		</div>

