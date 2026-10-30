plugins { id("com.android.application") }

android {
    namespace = "io.github.opaiva1721.narutodiag"
    compileSdk = 35
    ndkVersion = "28.2.13676358"
    defaultConfig {
        applicationId = "io.github.opaiva1721.narutodiag"
        minSdk = 29
        targetSdk = 35
        versionCode = 1
        versionName = "0.1.0-scaffold"
        ndk { abiFilters += listOf("arm64-v8a", "x86_64") }
    }
    externalNativeBuild {
        cmake { path = file("src/main/cpp/CMakeLists.txt"); version = "3.22.1" }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    lint { abortOnError = true }
}
