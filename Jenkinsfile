pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
                def commitHash = env.GIT_COMMIT
                def branch = env.GIT_BRANCH
                def previous = env.GIT_PREVIOUS_SUCCESSFUL_COMMIT
                sh './bin/build.sh ${commitHash} ${branch} ${previous}'
            }
        }
        stage ('Run') {
            steps {
                def commitHash = env.GIT_COMMIT
                def branch = env.GIT_BRANCH
                sh './bin/run.sh ${commitHash} ${branch}'
            }
        }
    }
}