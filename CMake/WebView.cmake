set(MSWEBVIEW2_VERSION "1.0.1150.38")

function(fetch_mswebview2 VERSION)
  cmake_policy(PUSH)

  # Avoid warning related to FetchContent and DOWNLOAD_EXTRACT_TIMESTAMP
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()

  if(NOT COMMAND FetchContent_Declare)
    include(FetchContent)
  endif()

  set(FC_NAME microsoft_web_webview2)
  FetchContent_Declare(${FC_NAME}
    URL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${VERSION}")
  FetchContent_MakeAvailable(${FC_NAME})

  set(MSWebView2_ROOT "${${FC_NAME}_SOURCE_DIR}")
  set(MSWebView2_ROOT "${MSWebView2_ROOT}" PARENT_SCOPE)

  cmake_policy(POP)
endfunction()

function(find_webview_libs)
  find_package(MSWebView2 QUIET)
  if(NOT MSWebView2_FOUND)
    fetch_mswebview2(${MSWEBVIEW2_VERSION})
  endif()

  find_package(MSWebView2 REQUIRED)
  if(MSWebView2_FOUND)
    list(APPEND WEBVIEW_DEPENDENCIES MSWebView2::headers)
  endif()
endfunction()
