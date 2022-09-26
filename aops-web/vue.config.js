const path = require('path')
const webpack = require('webpack')
const GitRevisionPlugin = require('git-revision-webpack-plugin')
const GitRevision = new GitRevisionPlugin()
const buildDate = JSON.stringify(new Date().toLocaleString())
const createThemeColorReplacerPlugin = require('./config/plugin.config')
// const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin

function resolve (dir) {
  return path.join(__dirname, dir)
}

// check Git
function getGitHash () {
  try {
    return GitRevision.version()
  } catch (e) {}
  return 'unknown'
}

// vue.config.js
const vueConfig = {
  configureWebpack: {
    // webpack plugins
    plugins: [
      // Ignore all locale files of moment.js
      new webpack.IgnorePlugin(/^\.\/locale$/, /moment$/),
      new webpack.DefinePlugin({
        APP_VERSION: `"${require('./package.json').version}"`,
        GIT_HASH: JSON.stringify(getGitHash()),
        BUILD_DATE: buildDate
      })
      // 依赖大小分析工具
      // new BundleAnalyzerPlugin()
    ],

    externals: {}
  },

  chainWebpack: config => {
    config.resolve.alias.set('@$', resolve('src'))

    const svgRule = config.module.rule('svg')
    svgRule.uses.clear()
    svgRule
      .oneOf('inline')
      .resourceQuery(/inline/)
      .use('vue-svg-icon-loader')
      .loader('vue-svg-icon-loader')
      .end()
      .end()
      .oneOf('external')
      .use('file-loader')
      .loader('file-loader')
      .options({
        name: 'assets/[name].[hash:8].[ext]'
      })
  },

  css: {
    loaderOptions: {
      less: {
        modifyVars: {
          // less vars，customize ant design theme

          // 'primary-color': '#F5222D',
          // 'link-color': '#F5222D',
          'border-radius-base': '2px'
        },
        // DO NOT REMOVE THIS LINE
        javascriptEnabled: true
      }
    }
  },

  devServer: {
    // development server port 8000
    port: 8000,
    // If you want to turn on the proxy, please remove the mockjs /src/main.jsL11
    proxy: {
      '/api/domain': {
        target: 'http://123.60.114.22:11114',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api/host': {
        target: 'http://123.60.114.22:11114',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api/confs': {
        target: 'http://123.60.114.22:11114',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api/management': {
        target: 'http://123.60.114.22:11114',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api/diag': {
        target: 'http://123.60.114.22:11113',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api/check': {
        target: 'http://123.60.114.22:11112',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      },
      '/api': {
        target: 'http://123.60.114.22:11111',
        ws: false,
        changeOrigin: true,
        pathRewrite: {
          '^/api': ''
        }
      }
    }
  },

  // disable source map in production
  productionSourceMap: false,
  lintOnSave: undefined,
  // babel-loader no-ignore node_modules/*
  transpileDependencies: []
}

// preview.pro.loacg.com only do not use in your production;
if (process.env.VUE_APP_PREVIEW === 'true') {
  console.log('VUE_APP_PREVIEW', true)
  // add `ThemeColorReplacer` plugin to webpack plugins
  vueConfig.configureWebpack.plugins.push(createThemeColorReplacerPlugin())
}

module.exports = vueConfig