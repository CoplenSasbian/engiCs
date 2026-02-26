# 将当前目录及子目录下所有 .cppm 文件重命名为 .ixx 扩展名

# 获取当前目录
$currentDir = Get-Location

# 递归查找所有 .cppm 文件
$files = Get-ChildItem -Path $currentDir -Filter "*.cppm" -Recurse -File

if ($files.Count -eq 0) {
    Write-Host "未找到任何 .cppm 文件。" -ForegroundColor Yellow
    exit
}

Write-Host "找到 $($files.Count) 个 .cppm 文件：" -ForegroundColor Green

# 显示将要重命名的文件
foreach ($file in $files) {
    Write-Host "  $($file.FullName)" -ForegroundColor Cyan
}

# 确认操作
$confirmation = Read-Host "`n确定要将这些文件重命名为 .ixx 扩展名吗？(Y/N)"
if ($confirmation -ne 'Y' -and $confirmation -ne 'y') {
    Write-Host "操作已取消。" -ForegroundColor Yellow
    exit
}

# 执行重命名操作
$renamedCount = 0
foreach ($file in $files) {
    try {
        $newName = Join-Path $file.DirectoryName ($file.BaseName + ".ixx")
        Rename-Item -Path $file.FullName -NewName $newName -ErrorAction Stop
        Write-Host "已重命名: $($file.Name) -> $($file.BaseName).ixx" -ForegroundColor Green
        $renamedCount++
    }
    catch {
        Write-Host "重命名失败: $($file.FullName) - $_" -ForegroundColor Red
    }
}

Write-Host "`n完成！成功重命名 $renamedCount 个文件。" -ForegroundColor Green