#Requires -Version 5.1
<#
.SYNOPSIS
    Gyeongseong97 릴리스 패키징 스크립트.

.DESCRIPTION
    솔루션을 빌드한 뒤 실행 파일과 런타임 리소스(Sounds, Sprites, stage_data.json),
    그리고 최상단 문서(CREDIT / README / THIRD_PARTY_NOTICES)를 하나의 zip으로 묶는다.
    결과물은 dist\ 폴더에 생성된다.

.PARAMETER Configuration
    빌드 구성. 기본값 Release.

.PARAMETER Platform
    빌드 플랫폼. 기본값 x64.

.PARAMETER SkipBuild
    빌드를 건너뛰고 기존 산출물로 패키징만 수행한다.

.EXAMPLE
    .\package.ps1
    .\package.ps1 -Configuration Debug
    .\package.ps1 -SkipBuild
#>
[CmdletBinding()]
param(
    [string]$Configuration = 'Release',
    [string]$Platform = 'x64',
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'

$RepoRoot   = $PSScriptRoot
$ProjectDir = Join-Path $RepoRoot 'Gyeongseong97'
$Solution   = Join-Path $RepoRoot 'Gyeongseong97.sln'

function Find-MSBuild {
    # Visual Studio Installer 의 vswhere 로 최신 MSBuild 탐색
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path $vswhere) {
        $found = & $vswhere -latest -requires Microsoft.Component.MSBuild `
            -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
        if ($found) { return $found }
    }
    # PATH 에 등록되어 있으면 그대로 사용
    $cmd = Get-Command MSBuild.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw 'MSBuild.exe 를 찾을 수 없습니다. Visual Studio 또는 Build Tools 를 설치하세요.'
}

# 1) 빌드
if (-not $SkipBuild) {
    $msbuild = Find-MSBuild
    Write-Host "[1/4] 빌드 ($Configuration|$Platform) - $msbuild"
    & $msbuild $Solution /t:Build /p:Configuration=$Configuration /p:Platform=$Platform `
        /m /nologo /verbosity:minimal
    if ($LASTEXITCODE -ne 0) { throw "빌드 실패 (exit code $LASTEXITCODE)" }
}
else {
    Write-Host '[1/4] 빌드 건너뜀 (-SkipBuild)'
}

# 2) 산출물 확인
$exe = Join-Path $RepoRoot "$Platform\$Configuration\Gyeongseong97.exe"
if (-not (Test-Path $exe)) {
    throw "실행 파일을 찾을 수 없습니다: $exe (먼저 빌드가 필요합니다)"
}

# 3) 스테이징 (zip 내부에 Gyeongseong97\ 폴더로 담기 위해 별도 디렉터리에 모은다)
Write-Host '[2/4] 파일 수집'
$DistDir  = Join-Path $RepoRoot 'dist'
$StageDir = Join-Path $DistDir 'Gyeongseong97'
if (Test-Path $StageDir) { Remove-Item $StageDir -Recurse -Force }
New-Item -ItemType Directory -Path $StageDir -Force | Out-Null

# 실행 파일
Copy-Item $exe -Destination $StageDir

# 런타임 리소스 (실행 파일과 같은 폴더에서 로드됨)
foreach ($folder in 'Sounds', 'Sprites') {
    $src = Join-Path $ProjectDir $folder
    if (-not (Test-Path $src)) { throw "리소스 폴더가 없습니다: $src" }
    Copy-Item $src -Destination $StageDir -Recurse
}

# 스테이지 데이터 (게임 구동에 필수)
Copy-Item (Join-Path $ProjectDir 'stage_data.json') -Destination $StageDir

# 문서
foreach ($doc in 'CREDIT.md', 'README.md', 'THIRD_PARTY_NOTICES.md') {
    $src = Join-Path $RepoRoot $doc
    if (-not (Test-Path $src)) { throw "문서를 찾을 수 없습니다: $src" }
    Copy-Item $src -Destination $StageDir
}

# 4) 압축
Write-Host '[3/4] 압축'
$stamp   = Get-Date -Format 'yyyyMMdd'
$zipPath = Join-Path $DistDir "Gyeongseong97_${Platform}_$stamp.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Compress-Archive -Path $StageDir -DestinationPath $zipPath -CompressionLevel Optimal

# 스테이징 폴더 정리
Remove-Item $StageDir -Recurse -Force

$sizeMB = [math]::Round((Get-Item $zipPath).Length / 1MB, 2)
Write-Host "[4/4] 완료: $zipPath ($sizeMB MB)"
