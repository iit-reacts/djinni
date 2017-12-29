using Djinni.TestSuite;
using NUnit.Framework;

namespace Djinni.Testing.Unit
{
    [TestFixture]
    public class TokenTest
    {
        private class CsToken : UserToken
        {
            public override string Whoami()
            {
                return "C#";
            }
        }

        [Test]
        public void TestTokens()
        {
            UserToken token = new CsToken();
            Assert.AreSame(token, TestHelpers.TokenId(token));
        }

        [Test]
        public void TestNullToken()
        {
            Assert.AreSame(null, TestHelpers.TokenId(null));
        }

        [Test]
        public void TestCppToken()
        {
            var token = TestHelpers.CreateCppToken();
            Assert.AreSame(token, TestHelpers.TokenId(token));
            Assert.That(() => TestHelpers.CheckCppToken(token), Throws.Nothing);
        }

        [Test]
        public void TestTokenType()
        {
            Assert.That(() => TestHelpers.CheckTokenType(new CsToken(), "C#"), Throws.Nothing);
            Assert.That(() => TestHelpers.CheckTokenType(TestHelpers.CreateCppToken(), "C++"), Throws.Nothing);
            Assert.That(() => TestHelpers.CheckTokenType(new CsToken(), "foo"), Throws.Exception);
            Assert.That(() => TestHelpers.CheckTokenType(TestHelpers.CreateCppToken(), "foo"), Throws.Exception);
        }

        [Test]
        public void TestNotCppToken()
        {
            Assert.That(() => TestHelpers.CheckCppToken(new CsToken()), Throws.Exception);
        }
}
}