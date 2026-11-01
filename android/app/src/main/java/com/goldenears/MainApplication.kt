package com.goldenears

import android.app.Application
import android.util.Log
import com.facebook.react.PackageList
import com.facebook.react.ReactApplication
import com.facebook.react.ReactHost
import com.facebook.react.ReactNativeApplicationEntryPoint.loadReactNative
import com.facebook.react.defaults.DefaultReactHost.getDefaultReactHost
import com.goldenears.bridge.AudioEngineBridge

class MainApplication : Application(), ReactApplication {

  override val reactHost: ReactHost by lazy {
    getDefaultReactHost(
      context = applicationContext,
      packageList =
        PackageList(this).packages.apply {
          // Packages that cannot be autolinked yet can be added manually here, for example:
          // add(MyReactNativePackage())
        },
    )
  }

  override fun onCreate() {
    super.onCreate()
    loadReactNative(this)
    initAudioEngine()
  }

  /**
   * Proves the JNI link at startup. initEngine() is O(1) and performs no IO, so
   * it is allowed on the main thread for now. When the engine opens an AAudio
   * stream, this call moves to a dedicated engine thread.
   */
  private fun initAudioEngine() {
    val initialized = AudioEngineBridge.initEngine()
    val status = AudioEngineBridge.getEngineStatus()
    Log.i(LOG_TAG, "initEngine=$initialized status=$status")
  }

  companion object {
    private const val LOG_TAG = "GoldenEars"
  }
}
