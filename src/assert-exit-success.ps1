#---------------------------------------------------------------------------------------------------
# Function definition
#---------------------------------------------------------------------------------------------------

function Assert-LastExitSuccess {
    Param(
        [Parameter(Mandatory=$True)]
        [string]$message
    )

    if($LastExitCode -ne 0)
    {
        throw [System.Exception] "$($MyInvocation.ScriptName):$($MyInvocation.ScriptLineNumber): ERROR: $message"
    }
}
